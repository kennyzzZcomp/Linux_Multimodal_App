#ifndef TY_INCLUDE_CONVSDK_H
#define TY_INCLUDE_CONVSDK_H

#include <stdlib.h>

#include <atomic>
#include <mutex>

#include "conv_code.h"
#include "conv_constants.h"

namespace convsdk {

class ConversationImpl;
class Conversation {
 public:
  Conversation();
  ~Conversation();

  /**
   * @brief 创建交互，设置回调。对应销毁接口为DestroyConversation
   * @param onMessage : 各事件监听回调，参见下文具体回调
   * @param onEventTrackMessage : 用于SDK内部日志(info以上)通过回调送给上层
   * @param user_data : 回调返回
   * @return Conversation* : Conversation单例指针
   */
  static Conversation* CreateConversation(
      const ConversationCallbackMethod onMessage,
      const EventTrackCallbackMethod onEventTrackMessage,
      void* user_data = NULL);

  /**
   * @brief 销毁交互，若交互进行中，则内部完成Disconnect()
   * @return ConvRetCode : 状态码
   */
  ConvRetCode DestroyConversation();

  /**
   * @brief 与服务端建立链接
   * @param params ：json string形式的初始化参数
   * @return ConvRetCode : 状态码
   */
  ConvRetCode Connect(const char* params);

  /**
   * @brief 断开链接
   * @return ConvRetCode : 状态码
   */
  ConvRetCode Disconnect();

  /**
   * @brief 按键(Tap)打断。正在播放时，调用此接口请求打断播放。
   * @return ConvRetCode : 状态码
   */
  ConvRetCode Interrupt();

  /**
   * @brief 推送音频数据
   * @param data : 音频数据，PCM格式
   * @param data_size : 音频数据字节数
   * @param type : 音频数据编码设置，暂未启用，设置无效。
   * @param timestamp : 此数据包的时间戳(ms)。
   * @return ConvRetCode : 状态码
   */
  ConvRetCode SendAudioData(const uint8_t* data, size_t data_size,
                            EncoderType type = kEncoderNone,
                            uint64_t timestamp = 0);

  /**
   * @brief 推送参考音频数据，在音频数据送给播放器时推送
   * @param data : 音频数据，PCM格式
   * @param data_size : 音频数据字节数
   * @param timestamp : 此数据包的时间戳(ms)。
   * @return ConvRetCode : 状态码
   */
  ConvRetCode SendRefData(const uint8_t* data, size_t data_size,
                          uint64_t timestamp = 0);

  /**
   * @brief
   * 通知服务端与用户主动交互，可以直接把上传的文本转换为语音下发，也可以上传文本调用大模型，返回的结果再转换为语音下发
   * @param params ：json string形式的初始化参数
   * @return ConvRetCode : 状态码
   */
  ConvRetCode SendResponseData(const char* params);

  /**
   * @brief
   * 向SDK送入通过RTC获得的VoiceChat相关response。response将在VideoChat Native
   * SDK内部进行解析，转成交互状态和相关事件给APP。
   * @param params ：json string形式的初始化参数
   * @return ConvRetCode : 状态码
   */
  ConvRetCode GetResponse(const char* params);

  /**
   * @brief 触发SDK动作，比如音频（播放）开始/结束事件告知到SDK
   * @param action : 触发的动作类型
   * @param data : 此次SDK动作附带的数据, 比如音频数据比如json字符串
   * @param data_size : 此次SDK动作附带的数据的字节数
   * @return ConvRetCode : 状态码
   */
  ConvRetCode SetAction(ConvAction action, const uint8_t* data = NULL,
                        size_t data_size = 0);

  /**
   * @brief 获得当前状态
   * @param type : 状态类型
   * @return ConvRetCode : 状态码
   */
  int GetState(StateType type);

  /**
   * @brief 发送如P2T相关的参数
   * @param params : 参数
   * @return ConvRetCode : 状态码
   */
  ConvRetCode UpdateMessage(const char* params);

  /**
   * @brief 获得当前SDK版本号
   * @return const char* : 版本信息
   */
  const char* GetVersion();

  /**
   * @brief 获得当前SDK中参数值
   * @param param : 需要获得参数名
   * @return const char* : 参数值，普通字符串或json字符串
   */
  const char* GetParameter(const char* param);

 private:
  static std::mutex instances_lock_;
  static std::atomic<bool> connecting_flag_;
  std::mutex instance_lock_;
  ConversationImpl* impl_;
};

}  // namespace convsdk

#endif  // TY_INCLUDE_CONVSDK_H
