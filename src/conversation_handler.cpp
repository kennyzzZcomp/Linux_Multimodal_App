#include "conversation_handler.h"
#include "audio_handler.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "json/json.h"

using namespace convsdk;

/**
 * @brief 语音对话初始化回调函数
 */
void onMessage(ConvEvent *event, void *param)
{
    ConvEvent::ConvEventType event_type = event->GetMsgType();
    //Conversation* conversation = static_cast<Conversation *>(param);
    int dialog_state = event->GetDialogStateChanged();

    // if (event_type != ConvEvent::kSoundLevel &&
    //     event_type != ConvEvent::kBinary)
    // {
    //     std::cout << "trigger onMessage -->>\n"
    //               << "  session id: " << event->GetSessionId() << "\n"
    //               << "  messge type: " << event->GetMsgTypeString() << "\n"
    //               << "  dialog state: " << dialog_state << "\n"
    //               << "  response: " << event->GetAllResponse() << std::endl;
    // }

    switch (event_type)
    {
    case ConvEvent::kConversationFailed:
        // 对话发生错误
        break;
    case ConvEvent::kConversationStarted:{
        // 对话建连成功
        std::cout<<"对话已开始!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        // push2talk 模式下不应等待 SentenceBegin 才开始发送音频：
        // SentenceBegin 通常由服务端对上行音频/VAD 判定触发，否则会造成“永远不触发发送”。
        std::thread audioSendThread([]() {
            // 等待 DialogStateChanged -> IDLE 的许可
            while (!can_send_audio.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            ConvRetCode start_ret = conversation->SetAction(kStartHumanSpeech);
            std::cout << "SetAction StartHumanSpeech ret=" << start_ret << std::endl;
            if (start_ret != kSuccess) {
                std::cerr << "StartHumanSpeech failed (ret=" << start_ret << "), skip sending audio." << std::endl;
                return;
            }

            // Give the SDK a brief moment to switch state before pushing audio.
            std::this_thread::sleep_for(std::chrono::milliseconds(300));

            bool success = SendAudioFile(
                conversation,
                "/home/zijian/linux_cpp_multimodal/Linux_Multimodal_App/audio_16k.pcm",
                "pcm",
                16000,
                640,
                false
            );
            ConvRetCode stop_ret = conversation->SetAction(kStopHumanSpeech);
            std::cout << "SetAction StopHumanSpeech ret=" << stop_ret << std::endl;

            if (success) {
                std::cout << "✅ 音频流发送线程完成" << std::endl;
            } else {
                std::cerr << "❌ 音频流发送失败" << std::endl;
            }
        });
        audioSendThread.detach();
        break;
    }
    case ConvEvent::kConversationCompleted:
        // 对话完成
        std::cout<<"对话已完成!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        break;
    case ConvEvent::kSentenceBegin:{
        // 检测到用户开始说话, 这里可以启动录音采集音频
        std::cout<<"收到SentenceBegin事件，用户开始说话。" << std::endl;
        break;

    }
    case ConvEvent::kSentenceEnd:
        // 检测到用户说话结束, 这里可以停止录音采集音频
        std::cout<<"收到SentenceEnd事件，用户结束说话。" << std::endl;
        //can_send_audio = true;
        break;
    case ConvEvent::kDataOutputStarted:
        // 后续将接收语音合成数据, 这里可启动播放器。
        // 播放器启动后需要通知SDK
        std::cout<<"收到DataOutputStarted事件，通知SDK播放器已启动播放。" << std::endl;
        conversation -> SetAction(kPlayerStarted);
        break;
    case ConvEvent::kDataOutputCompleted:
        // 接收语音合成数据完成, 这里需要通知播放器已经送完数据。
        // 注意, 这里只是接收完语音合成数据, 而非播放完成, 缓存或播放器中还有大量数据待播放。
        // 完全播放完后必须通知SDK
        std::cout<<"收到DataOutputCompleted事件，通知SDK播放器已完成播放。" << std::endl;
        conversation -> SetAction(kPlayerStopped);
        break;
    case ConvEvent::kBinary:{
        int n = event->GetBinaryDataSize();
        std::cout << "RECEIVE RESPONSE trigger onMessage -->> kBinary, session: " << event->GetSessionId()
                    << ", bytes=" << n << std::endl;
        // 保存下发的二进制音频（例如 TTS 音频）到本地，便于播放/调试
        SaveBinaryEventToFile(event);
        break;
    }
    case ConvEvent::kSoundLevel:
        break;
    case ConvEvent::kDialogStateChanged:{
        // 可通过对话状态进行相关业务逻辑操作
        int state = event->GetDialogStateChanged();
        std::cout << "Dialog state changed to :::: " << state << std::endl;
        switch (state)
        {
        case 0:
            /* code */
            std::cout << "Dialog State: IDLE" << std::endl;
            // allow sending audio now
            can_send_audio.store(true);
            break;
        case 1:
            std::cout << "Dialog State: LISTENING" << std::endl;
            break;
        case 2:
            std::cout << "Dialog State: RESPONDING" << std::endl;
            break;
        case 3:
            std::cout << "Dialog State: THINKING" << std::endl;
            break;
        default:
            break;
        }
    }
        break;
    case ConvEvent::kInterruptAccepted:
        // 允许打断, 这里可以关闭正在播放的音频
        break;
    case ConvEvent::kInterruptDenied:
        break;
    case ConvEvent::kVoiceInterruptAccepted:
        // 允许打断, 这里可以关闭正在播放的音频
        break;
    case ConvEvent::kVoiceInterruptDenied:
        break;
    case ConvEvent::kHumanSpeakingDetail:
        break;
    case ConvEvent::kRespondingDetail:
        std::cout << "Responding Detail: " << event->GetAllResponse() << std::endl;
        break;
    }
}
/**
 * @brief SDK日志回调函数
 */
void onEtMessage(ConvLogLevel level, const char *log, void *user_data)
{
    std::cout << " ==>> [" << level << "] " << log << std::endl;
}

std::string gen_init_params()
{
    Json::FastWriter writer;
    Json::Value root;
    Json::Value agent_chat;
    root["mode"] = g_mode;
    root["chain_mode"] = "ws";
    root["ws_version"] = 3;

    root["debug_mode"] = "normal";
    root["workspace"] = "/home/zijian/linux_cpp_multimodal/Linux_Multimodal_App/resources_aec_kws_vad_android";
    root["save_log"] = true;
    root["save_wav"] = true;
    root["log_level"] = g_log_level;
    // 必填参数:模型信息
    root["url"] = "wss://dashscope.aliyuncs.com/api-ws/v1/inference";
    root["apikey"] = "sk-cebc306c1a7d44579af8d99c199789a2";
    root["app_id"] = "mm_62600140f1b743d7bad25d552b78";
    root["workspace_id"] = "llm-2d2jbauuwkp1250n";
    root["debug_path"] = "tmp/";

    Json::Value upstream;
    upstream["type"] = "AudioOnly";
    // 对齐官方 demo：上行设置为 opus（SDK 内部会对 SendAudioData 的 PCM 做编码），避免服务端认为 payload 无效。
    upstream["audio_format"] = "pcm"; // asr格式，支持pcm,opus,raw-opus
    upstream["sample_rate"] = 16000;
    root["upstream"] = upstream;

    Json::Value downstream;
    downstream["type"] = "Audio";
    downstream["voice"] = "longanhuan";
    downstream["sample_rate"] = 24000;  // default tts sample_rate is 24000
    downstream["audio_format"] = "pcm"; // 下发的音频编码格式，支持opu,pcm
    downstream["intermediate_text"] = "transcript,dialog";
    downstream["debug"] = true;
    root["downstream"] = downstream;

    Json::Value client_info; /* 额外扩充的用户信息 */
    client_info["user_id"] = "bin23439207";
    root["client_info"] = client_info;

    Json::Value dialog_attributes;
    dialog_attributes["prompt"] = "你是个有用的助手。";
    root["dialog_attributes"] = dialog_attributes;

    Json::Value acoustic_echo_cancelling_attributes;
    acoustic_echo_cancelling_attributes["enable_external_aec_module"] = false;
    root["acoustic_echo_cancelling_attributes"] =
        acoustic_echo_cancelling_attributes;

    Json::Value voice_activity_detection_attributes;
    voice_activity_detection_attributes["enable_external_vad_module"] = false;
    root["voice_activity_detection_attributes"] =
        voice_activity_detection_attributes;

    Json::Value key_words_spotting_attributes;
    key_words_spotting_attributes["enable_external_kws_module"] = false;
    key_words_spotting_attributes["independent_kws_mode"] = false;
    root["key_words_spotting_attributes"] = key_words_spotting_attributes;

    std::string result = writer.write(root);
    std::cout << "init params: " << result.c_str() << std::endl;
    return result;
}