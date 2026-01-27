#ifndef CONVSDK_CONSTANTS_H
#define CONVSDK_CONSTANTS_H

#include "conv_event.h"

namespace convsdk {

enum ConvMode {
  kModeTapToTalk,
  kModeMixDuplex,
  kModePushToTalk,
  kModeMixKwsDuplex,
};

enum ConvChainMode {
  kChainModeWebsocket,
  kChainModeRTC,
};

enum StateType {
  kTypeDialogState,
  kTypeConnectionState,
  kTypeInterruptionPolicy,
  kTypeMicState,
  kTypePlayerState,
  kTypeAppAction,
};

enum ConvDialogState {
  kDialogIdle,
  kDialogListening,
  kDialogResponding,
  kDialogThinking,
};

enum ConvSpeechState {
  kSpeechSilent,      /* no voice */
  kSpeechKwsAsleep,   /* human speaking, but keyword spotting not awake */
  kSpeechKwsAwake,    /* keyword spotting is awake, but no voice subsequently */
  kSpeechKwsAwakeNew, /* keyword spotting is awake, next dialog round, but no
                         voice subsequently */
  kSpeechRecognition, /* doing speech recognizing */
};

enum ConvConnectionState {
  kDialogDisconnected,
  kDialogConnected,
};

enum ConvInterruptionPolicy {
  kInterruptionPolicyNone,
  kInterruptionPolicyAccept,
  kInterruptionPolicyDeny,
  kInterruptionPolicyAsk,
};

enum ConvAction {
  kMicStarted = 0, /* reserved */
  kMicStopped,     /* reserved */
  /* Indicates that the player has started playing */
  kPlayerStarted,
  /* Notify the player that the playback is complete, and the SDK will enter the
     next step of interaction. */
  kPlayerStopped,
  kAppEnterBackground, /* reserved */
  kAppEnterForeground, /* reserved */
  /* Turn on voice interrupt */
  kEnableVoiceInterruption,
  /* Turn off voice interrupt */
  kDisableVoiceInterruption,
  /* Start mute and the SDK will reject all input audio. */
  kVoiceMute,
  /* Stop mute, and return to normal interaction mode. */
  kVoiceUnmute = 9,
  /* The speaker starts to speak and initiates to send upstream data to the
     server. P2T mode is used. */
  kStartHumanSpeech,
  /* Indicates that the person has finished speaking, and notifies the server to
     proceed to the next interaction. P2T mode is used. */
  kStopHumanSpeech,
  /* It means that the current speaking content is canceled. P2T mode is used,
     and this function is not enabled at present. */
  kCancelHumanSpeech,
  /* Invoking this function when the earphone are unplugged will optimize the
     internal echo cancellation effect. */
  kPullOutHeadphones = 13,
  /* In KWS_DUPLEX mode and single-step, this function notifies the SDK for the
     next step of interaction. */
  kContinue = 14,
  /* The external VAD module triggers the VAD BEGIN. */
  kVoiceDetectedBegin,
  /* The external VAD module triggers the VAD END. */
  kVoiceDetectedEnd,
  /* The external VAD module triggers a timeout. */
  kVoiceTimeout,
  /* The external wake-up module triggers the wake-up word */
  kKeywordTrusted,
  /* Enter monologue mode and continue to stream up and down, and reject
     interruptions. */
  kEnableMonologueMode,
  /* Exit monologue mode, and return to normal interaction mode. */
  kDisableMonologueMode,
};

enum ConvLogLevel {
  kConvLogLevelVerbose,
  kConvLogLevelDebug,
  kConvLogLevelInfo,
  kConvLogLevelWarning,
  kConvLogLevelError,
  /* No logs are output. */
  kConvLogLevelNone,
};

enum EncoderType {
  kEncoderNone = 0,
  kEncoderOggOpus,  // OPUS data with OGG header
  kEncoderOpus,     // useless
  kEncoderOpu,      // opus + 1bytes(data size) in head
  kEncoderOpu2,     // opus + 2bytes(data size) in head
  kEncoderRawOpus,  // raw opus
};

enum DecoderType {
  kDecoderNone = 0,
  kDecoderOggOpus,  // OPUS data with OGG header
  kDecoderOpus,     // useless
  kDecoderOpu,      // opus + 1bytes(data size) in head
  kDecoderOpu2,     // opus + 2bytes(data size) in head
  kDecoderRawOpus,  // raw opus
};

enum ChannelSelection {
  kFirstChannel = 0,
  kSecondChannel,
  kStereoChannel,
};

/**
 * @brief SDK各事件回调
 * @param ConvEvent*
 * ：回调事件，可通过调用其中具体方法获得事件及相关信息。具体方法见下方ConvEvent
 * @param void* ：用户设置的user_data
 */
typedef void (*ConversationCallbackMethod)(ConvEvent*, void*);
/**
 * @brief SDK内部日志通过回调送给用户处理
 * @param ConvLogLevel ：返回日志的日志级别
 * @param const char* ：具体日志字符串
 * @param void* ：用户设置的user_data
 */
typedef void (*EventTrackCallbackMethod)(ConvLogLevel, const char*, void*);

}  // namespace convsdk

#endif  // CONVSDK_CONSTANTS_H
