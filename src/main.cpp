#include <iostream>
#include <cstring>
#include <signal.h>
#include <string>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <pthread.h>

#include "conversation_handler.h"
#include "audio_handler.h"

#include "conversation.h"
#include "conversation_utils.h"
#include "json/json.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <ctime>

using namespace convsdk;

static std::string g_apikey = "";
static std::string g_url = "wss://dashscope.aliyuncs.com/api-ws/v1/inference";
std::string g_log_level = "verbose"; /* version, debug, info, warn, error */
std::string g_mode = "push2talk"; /* tap2talk, push2talk, duplex, kws_duplex */
std::string g_exeDir = getExecutableDirectory();
std::string audio_file_path = g_exeDir + "/audio_16k.pcm";
std::string g_image_file_path = g_exeDir + "/test_img.jpg";

Conversation *conversation;
std::atomic<bool> can_send_audio{false};

struct ConnectThreadResult {
    ConvRetCode ret{convsdk::kDefaultError};
};

static void* ConnectThreadMain(void* arg) {
    auto* result = static_cast<ConnectThreadResult*>(arg);
    if (!result) return nullptr;

    ConversationCallbackMethod on_message_callback = onMessage;
    EventTrackCallbackMethod on_et_callback = onEtMessage;

    conversation = Conversation::CreateConversation(on_message_callback, on_et_callback, NULL);
    if (!conversation) {
        result->ret = convsdk::kNotCreateConversation;
        return nullptr;
    }

    std::string params = gen_init_params();
    result->ret = conversation->Connect(params.c_str());
    if (result->ret != convsdk::kSuccess) {
        conversation->DestroyConversation();
        conversation = NULL;
    }
    return nullptr;
}

/**
 * @brief 处理中断ctrlc
 */
void signalHandlerINT(int signum)
{
    std::cout << "\n收到信号 " << signum << "，准备退出..." << std::endl;
    std::exit(0);
}

/**
 * @brief 处理退出中断quit
 */
void signalHandlerQUIT(int signum)
{
    std::cout << "\n收到信号 " << signum << "，准备退出..." << std::endl;
    std::exit(0);
}

/**
 * @brief 解析命令行参数
 */
int parse_arg(int argc, char *argv[])
{
    int index = 1;
    while (index < argc)
    {
        if (!strcmp(argv[index], "--help"))
        {
            std::cout << "Usage: --apikey <key> [--url <wss-url>]" << std::endl;
            return 1;
        }
        else if (!strcmp(argv[index], "--apikey"))
        {
            index++;
            if (index >= argc)
            {
                std::cerr << "--apikey requires a value" << std::endl;
                return 1;
            }
            g_apikey = argv[index];
        }
        else if (!strcmp(argv[index], "--url"))
        {
            index++;
            if (index >= argc)
            {
                std::cerr << "--url requires a value" << std::endl;
                return 1;
            }
            g_url = argv[index];
        }
        else
        {
            std::cout << "unknown arg: " << argv[index] << std::endl;
        }
        index++;
    }

    if (g_apikey.empty())
    {
        std::cerr << "--apikey is required" << std::endl;
        return 1;
    }
    return 0;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    // Parse command line arguments
    if (parse_arg(argc, argv))
    {
        return -1;
    }
    std::cout << "parsed apikey: " << g_apikey << std::endl;
    std::cout << "using url: " << g_url << std::endl;

    signal(SIGINT, signalHandlerINT);
    signal(SIGQUIT, signalHandlerQUIT);

    // Create + Connect in a dedicated pthread
    pthread_t connect_thread;
    ConnectThreadResult connect_result;
    int cret = pthread_create(&connect_thread, nullptr, &ConnectThreadMain, &connect_result);
    if (cret != 0) {
        std::cerr << "pthread_create failed: " << cret << std::endl;
        return -1;
    }
    pthread_join(connect_thread, nullptr);

    ConvRetCode ret = connect_result.ret;
    if (ret != convsdk::kSuccess)
    {
        std::cout << "connect failed: " << ret;
        if (conversation) {
            std::cout << " with " << conversation->GetVersion();
        }
        std::cout << std::endl;
        return -1;
    }

    std::cout << "connect success: " << std::endl;
    // 进入 CLI 等待用户输入指令
    std::cout << "CLI commands: 1=send audio, q=quit, help=show commands" << std::endl;
    for (std::string cmd;;) {
        std::cout << ">> " << std::flush;
        if (!std::getline(std::cin, cmd)) {
            break;
        }
        if (cmd == "1") {
            // send recorded audio file
            trigger_audio_send_once(audio_file_path);
        }else if (cmd == "2") {
            // request to have tts respond
            text_to_speech_request("幸福是一种技能，是你摒弃了外在多余欲望后的内心平和。");
        }else if(cmd == "3"){
            // VQA request
            // replace with your image path
            std::string image_path = g_image_file_path; 
            vqa_send_request(image_path);
        } else if (cmd == "help") {
            std::cout << "CLI commands: 1=send audio, q=quit, help=show commands" << std::endl;
        } else if (cmd == "q" || cmd == "quit" || cmd == "exit") {
            break;
        } else if (!cmd.empty()) {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }

    std::cout << "\n 断开连接..." << std::endl;
    ret = conversation->Disconnect();
    if (ret == 0)
    {
        std::cout << "disconnect success" << std::endl;
    }
    // 销毁Conversation实例，释放资源
    conversation->DestroyConversation();
    conversation = NULL;

    return 0;
}
