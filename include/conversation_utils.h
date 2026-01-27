#ifndef TY_INCLUDE_CONVSDK_UTILS_H
#define TY_INCLUDE_CONVSDK_UTILS_H

#include <string>
#include <vector>

namespace convsdk {

class ConversationUtilsImpl;
class ConversationUtils {
 public:
  ConversationUtils();
  ~ConversationUtils();

  static ConversationUtils* CreateConversationUtils();

  void DestroyConversationUtils(void* handler);

  int TryCreateAudioEncoder(std::string type, int channels,
                            const int sample_rate, int* errorCode);

  int RecreateAudioEncoder(std::string type, int channels,
                           const int sample_rate, int* errorCode);

  int DestroyAudioEncoder();

  int AudioEncoderSoftRestart();

  /**
   * @brief 对数据进行编码
   * @param frameBuff 原始PCM音频数据
   * @param frameLen 原始PCM音频数据长度。目前仅支持640字节。
   * @param outputBuffer 装载编码后音频数据的数组
   * @param outputSize outputBuffer长度
   * @return 成功返回opu编码后的数据长度，失败返回opus错误代码
   */
  int AudioEncoding(const uint8_t* frameBuff, const int frameLen,
                    unsigned char* outputBuffer, int outputSize);

  /**
   * @brief 获得用于编码的每帧字节长度, 即以FrameSampleBytes大小数据进行编码
   * @return 成功返回每帧字节长度，失败返回负值
   */
  int GetFrameSampleBytes();

  /**
   * @brief 设置编码的每帧字节长度, 即以FrameSampleBytes大小数据进行编码
   * @return 成功返回0，失败返回负值
   */
  int SetFrameSampleBytes(int bytes);

  int TryCreateAudioDecoder(std::string type, int channels,
                            const int sample_rate, int* errorCode);

  int RecreateAudioDecoder(std::string type, int channels,
                           const int sample_rate, int* errorCode);

  int DestroyAudioDecoder();

  /**
   * @brief 对数据进行解码
   * @param frameBuff 待解码的数据
   * @param frameLen 待解码的数据的字节数
   * @param outputBuffer 解码数据存储的输出缓存
   * @param outputBufferBytes 解码数据存储的输出缓存的字节数.
   * 解码产生的数据多于这个值则会舍去超过的数据.
   * 默认值0则内部不检查解码产生的数据长度. 16K数据推荐设置20倍的frameLen,
   * 48k数据推荐设置60倍的frameLen.
   * @return
   */
  int AudioDecoding(const uint8_t* frameBuff, const int frameLen,
                    uint8_t* outputBuffer, int outputBufferBytes = 0);

  std::string Base64Encode(const std::vector<uint8_t>& data);

  std::string Base64EncodeFromFilePath(const std::string filePath);

  std::string RandomUUID();

 private:
  ConversationUtilsImpl* impl_;
};

}  // namespace convsdk

#endif  // TY_INCLUDE_CONVSDK_UTILS_H
