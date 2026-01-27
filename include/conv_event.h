#ifndef CONVSDK_INCLUDE_CONV_EVENT_H
#define CONVSDK_INCLUDE_CONV_EVENT_H

#include <list>
#include <string>
#include <vector>

#include "conv_code.h"

namespace convsdk {

class ConvEvent {
 public:
  enum ConvEventType {
    kConversationInvalid = -1,
    kConversationFailed = 0,

    /**
     * 与AI(服务端)建连成功
     */
    kConversationConnected,
    /**
     * AI(服务端)初始化完成, 返回初始化信息
     */
    kConversationInitialized,
    /**
     * AI(服务端)登场, 开始会话任务
     * "Started"
     */
    kConversationStarted,
    /**
     * AI(服务端)退场, 会话任务完成, 链接断开
     * "Stopped"
     */
    kConversationCompleted,
    /**
     * 检测HUMAN开始说话, 由端侧VAD检测到起点触发
     * "SpeechBegin" or "SpeechStarted"
     */
    kSentenceBegin,
    /**
     * HUMAN说话结束, 由AI(服务端)判停
     * "SpeechEnded"
     */
    kSentenceEnd,
    /**
     * AI(服务端)开始传回TTS数据
     * "RespondingStarted"
     */
    kDataOutputStarted,
    /**
     * AI(服务端)传回TTS数据完成
     * "RespondingEnded"
     */
    kDataOutputCompleted,
    /**
     * 表示此ConvEvent中包含AI(服务端)传回的TTS数据包
     * "Binary"
     */
    kBinary,
    /**
     * 最近一次传入的HUMAN说话声音数据的音量值
     */
    kSoundLevel,

    // Protocol 3.0
    /**
     * 对话状态发生变化
     * "DialogStateChanged"
     */
    kDialogStateChanged = 11,
    /**
     * 轻触打断被允许
     * "RequestAccepted"
     */
    kInterruptAccepted,
    /**
     * 轻触打断被拒绝
     * "RequestDenied"
     */
    kInterruptDenied,
    /**
     * 语音打断被允许
     */
    kVoiceInterruptAccepted,
    /**
     * 语音打断被拒绝
     */
    kVoiceInterruptDenied,
    /**
     * SDK与服务端断开链接
     */
    kConnectionDisconnected,
    /**
     * SDK与服务端链接成功
     */
    kConnectionConnected,

    // Protocol for Paris Olympics
    /**
     * 用户语音识别出的详细信息，比如文本
     * "SpeechContent"
     */
    kHumanSpeakingDetail,
    /**
     * 系统对外输出的详细信息，比如文本
     * "RespondingContent"
     */
    kRespondingDetail,
    /**
     * 网络状态信息, 比如网络延迟
     * "NetworkStatus"
     */
    kNetworkStatus = 20,
    /**
     * 本地唤醒模块触发唤醒
     */
    kKeywordSpotted,
    /**
     * 确认确实唤醒
     */
    kKeywordTrusted,
    /**
     * 长时间无人说话, 回到某初始状态
     */
    kVoiceTimeout,

    /**
     * 未定义的消息信息
     */
    kOtherMessage = 29,

    /**
     * !!!--- 以下为送给上层RTC SDK的事件 ---!!!
     */
    /**
     * 表示此ConvEvent中包含HUMAN(用户)传回的MIC数据包
     * "UserBinary"
     */
    kUserBinary = 30,
    /**
     * 表示将事件整理成RTC消息
     * "RTCMessage"
     */
    kRTCMessage,

    /**
     * !!!--- 以下为送给上层RTC SDK的事件请求, 将会转成kRTCMessage ---!!!
     */
    /**
     * "Start" or "StartedReceived"
     */
    kConversationStart,
    /**
     * "Stop"
     */
    kConversationStop,
    /**
     * "SendSpeech"
     */
    kSendSpeech,
    /**
     * "StopSpeech"
     */
    kStopSpeech,
    /**
     * "RequestToSpeak"
     */
    kRequestToSpeak,
    /**
     * "RequestToRespond"
     */
    kRequestToRespond,
    /**
     * "LocalRespondingStarted"
     */
    kLocalRespondingStarted,
    /**
     * "LocalRespondingEnded"
     */
    kLocalRespondingEnded,
    /**
     * "AvatarStateChanged"
     */
    kAvatarStateChanged,

    KValidEventCut,
  };

  enum NetworkEventType {
    kNetworkEventUnknown = 0,
    kNetworkEventLatency,
  };

  ConvEvent();
  ConvEvent(const ConvEvent& event, std::string task_id);
  ConvEvent(const ConvEvent& event, std::string task_id,
            std::string dialog_or_session_id);
  ConvEvent(const ConvEvent& event);
  explicit ConvEvent(std::string msg);
  ConvEvent(std::string msg, std::string task_id, std::string local_task_id);
  ConvEvent(std::string msg, std::string err_msg, int err_code);
  ConvEvent(std::string msg, std::string err_msg, int err_code, bool terminate,
            std::string local_task_id);
  ConvEvent(std::vector<unsigned char> data, std::string msg,
            std::string local_task_id);
  ConvEvent(std::vector<unsigned char> data, ConvEventType type,
            int sample_rate, int channel_select);
  explicit ConvEvent(ConvEventType type);
  ConvEvent(ConvEventType type, std::string task_id,
            std::string dialog_or_session_id, std::string local_task_id);
  ConvEvent(float sound_db, int sound_level, std::string dialog_or_session_id);
  ~ConvEvent();

  ConvEvent& operator=(const ConvEvent& rhs);

  /**
   * @brief 获取状态码
   * @note
   * 正常情况为0或者20000000，失败时对应失败的错误码。错误码参考SDK文档说明。
   * @return int
   */
  int GetStatusCode();

  /**
   * @brief 获得此ConvEvent的完整json格式response
   * @return const char*
   */
  const char* GetAllResponse();

  /**
   * @brief 覆盖更新ConvEvent的msg
   */
  void UpdateAllResponse(const char* new_response);

  /**
   * @brief 获得错误信息
   * @return const char*
   */
  const char* GetErrorMessage();

  /**
   * @brief 本次连接唯一标识，用于在工程链路上跟踪任务执行。
   * @note 定位问题建议用dialog_id和round_id
   * @return const char*
   */
  const char* GetTaskId();

  /**
   * @brief 获取会话的session id
   * @return const char*
   */
  const char* GetSessionId();

  /**
   * @brief 获取会话的dialog id, 同session id, 根据版本不同id命名不同
   * @return const char*
   */
  const char* GetDialogId();

  /**
   * @brief 本轮交互的ID
   * @return const char*
   */
  const char* GetRoundId();

  /**
   * @brief 获取云端返回的二进制数据
   * @note 仅用于语音合成功能
   * @return vector<unsigned char>
   */
  std::vector<unsigned char> GetBinaryData();

  /**
   * @brief 同GetBinaryData()
   */
  unsigned char* GetBinaryDataInChar();

  /**
   * @brief 同GetBinaryData(), 获得这一次二进制数据的字节数
   * @note 与GetBinaryDataInChar()搭配
   * @return int
   */
  int GetBinaryDataSize();

  void SetMsgType(ConvEventType type);

  /**
   * @brief 获取当前所发生Event的类型
   * @return EventType
   */
  ConvEventType GetMsgType();

  /**
   * @brief 当前所发生Event的类型的对应字符串
   * @note 若入参设置ConvEventType, 则返回此入参对应字符串
   * @return const char*
   */
  const char* GetMsgTypeString(int type = -1);

  /**
   * @brief 获取当前所发生Event的子类型
   * @return EventType
   */
  ConvEventType GetSubMsgType();

  /**
   * @brief 获得对话当前状态, 目前可不关注
   * @return int, ConvDialogState转换
   */
  int GetDialogStateChanged();

  /**
   * @brief 设置对话状态, 目前可不关注
   */
  void SetDialogStateChanged(int dialog_state);

  /**
   * @brief 获得打断策略, 目前可不关注
   * @return int, ConvInterruptionPolicy转换
   */
  int GetInterruptionPolicy();

  /**
   * @brief
   * 是否中断，true的话表示当前会话中断，需要disconnect()后再connect()重新开始会话
   * @return bool
   */
  bool GetTerminate();

  /**
   * @brief 返回最近一次输入音频的音量db值, 范围-160~0db
   * @return float
   */
  float GetSoundDb();

  /**
   * @brief 返回最近一次输入音频的音量db值的分级, 范围0~100
   * @note 对应GetSoundDb()
   * @return int
   */
  int GetSoundLevel();

  /**
   * @brief 返回此数据的采样率
   * @return int
   */
  int GetSampleRate();

  /**
   * @brief 返回音频数据的通道选择, 0:表示第一通道, 1:表示第二通道, 2:表示双通道
   * @return int
   */
  int GetChannelSelect();

  /**
   * @brief 返回网络延迟信息, 单位为毫秒
   * @return NetworkEventType
   */
  NetworkEventType GetNetworkEvent();

  /**
   * @brief 返回网络延迟信息, 单位为毫秒
   * @return int
   */
  int GetNetworkLatency();

 private:
  int ParseMsgType(std::string name);
  void TransferMsgType(int event);
  bool ParseResponse(std::string response);
  void InsertLocalTaskId(std::string response);

  int status_code_;
  std::string msg_;
  std::string error_msg_;
  ConvEventType msg_type_;
  ConvEventType sub_msg_type_;
  std::string msg_type_str;
  int dialog_state_;
  int interruption_policy_;
  /*
   * 百炼(DashScope)服务中表示本次连接唯一标识，即每次connect()产生一个ID。非百炼服务task_id意义与round_id相同
   */
  std::string task_id_;
  /* SDK内部产生的ID，表示本轮交互的ID，用于在服务未产生ID前定位问题。*/
  std::string local_task_id_;
  /* 对话ID，
   * 非百炼(DashScope)服务有效，每次connect()传入dialog_id可继续上轮对话，否则表示新对话。
   */
  std::string session_id_;
  /* 对话ID，
   * 百炼(DashScope)服务有效，每次connect()传入dialog_id可继续上轮对话，否则表示新对话。
   */
  std::string dialog_id_;
  /*
   * 只在百炼(DashScope)服务有用，表示本轮交互的ID，即每次用户说话为一轮产生一个ID。
   */
  std::string round_id_;
  std::string result_;
  bool terminate_;
  std::vector<unsigned char> binary_data_;
  float sound_db_;
  int sound_level_;
  int sample_rate_;
  int channel_select_;
  NetworkEventType network_event_;
  int network_latency_;
};

}  // namespace convsdk

#endif  // ACSDK_INCLUDE_AC_EVENT_H
