#ifndef CONVSDK_CONV_CODE_H
#define CONVSDK_CONV_CODE_H

namespace convsdk {

/*
 * error code of Conversation
 */
enum ConvRetCode {
  kSuccess = 0,

  /* common 10 - 99 */
  kDefaultError = 10,
  kMemAllocError,
  kJsonParseFailed,
  kUnsupportedMode,

  /* invoke 50 - 99 */
  kInitFailed = 50,
  kNotConnected,
  kEngineNull,
  kIllegalParam,
  kIllegalInitParam,
  kInvalidState,
  kHasInvoked,
  kNotCreateConversation,
  kSkipSendData,
  kSendDataFailed,
  kInvalidAudioData = 60,
  kStopFailed,
  kCancelFailed,
  kInvokeTimeout,
  kRequestDenied,
  kRequestAccepted,
  kSkipDestroyConversation,
  kInvokeInvalidAction,

  /* cei 100 - 109 */
  kCeiInitFailed = 100,
  kCeiSetParamFailed,
  kCeiStopFailed,    // useless
  kCeiCancelFailed,  // useless

  /* vad 110 - 139 */
  kVadSdkOk = 110,
  kVadSdkFileNotFound,
  kVadSdkInvalidParam,
  kVadSdkInvalidState,
  kVadSdkProcessFailed,
  kVadSdkCreateFailed,

  /* aec 140 - 169 */
  kAecSdkOk = 140,
  kAecSdkInvalidParam,
  kAecSdkInvalidState,
  kAecSdkProcessFailed,
  kAecSdkCreateFailed,

  /* kws 170 - 189 */
  kKwsSdkOk = 170,
  kKwsSdkInvalidParam,
  kKwsSdkInvalidState,
  kKwsSdkCreateFailed,
  kKwsSdkDestroyFailed,
  kKwsSdkStartFailed,
  kKwsSdkStopFailed,
  kKwsSdkUpdateDataFailed,

  /* conversation event 200 - 299 */
  kEmptyConvEvent = 200,
  kInvalidConvEventMsgType,

  /* net 300 - 399 */
  kWsConnectFailed = 300,  // useless
  kServerNotAccess,
  kAuthFailed,           // useless
  kCloseSocketFailed,    // useless
  kServerInternalError,  // useless
  kNoSupportMode = 310,
  kJsonFormatError,
  kSslError,
  kSslConnectFailed,
  kHttpConnectFailed,
  kDnsFailed,
  kSocketClosed,   // useless
  kPingNoSupport,  // useless
  kUtf8Error,
  kSystemCallFailed,
  kProtocolError,
  kParametersError,  // useless
  kHttpdnsFailedWarnning,

  /* nls 350 - 399 */
  kNlsIsNullptr = 350,
  kNlsInvokeTimeout,
  kNlsStartFailed,
  kNlsStopFailed,
  kNlsCancelFailed,
  kNlsStartInboundFailed,
  kNlsStopInboundFailed,
  kNlsStopOutboundFailed,
  kNlsCancelBoundFailed,
  kNlsSetParamsFailed,
  kNlsSendAudioFailed = 360,
  kNlsControlFailed,

  /* codec 400 - 499 */
  kEncoderExistent = 400, /* NLS的编码器已存在 */
  kDecoderExistent,
  kEncoderInexistent, /* NLS的编码器不存在 */
  kDecoderInexistent,
  kOpusEncoderCreateFailed, /* Opus编码器创建失败 */
  kOpusDecoderCreateFailed,
  kOggOpusEncoderCreateFailed, /* OggOpus编码器创建失败 */
  kInvalidEncoderType,         /* encoder类型无效 */

  /* codec - oggopus internal */
  kOggOpusInvalidState = 420, /* OggOpus状态不正确 */
  kOggOpusCreateFailed,       /* OggOpus创建失败 */
  kOggOpusStartFailed,        /* OggOpus启动失败 */
  kOggOpusEncodeFailed,       /* OggOpus编码失败 */
  kOggOpusDecodeFailed,       /* OggOpus解码失败 */
  kOggOpusStopFailed,         /* OggOpus停止失败 */
  kOggNoAvailablePage = 426,  /* Ogg无有效页 */
  kOggNoAvailablePacket,
  kOggInitFailed,           /* Ogg组件初始化失败 */
  kOggAllocateMemoryFailed, /* Ogg同步缓存申请失败 */
  kOggBufferOverflow,       /* Ogg写入缓存失败 */
  kOggStreamPageinFailed,   /* Ogg页信息写入失败 */
  kOpusInvalidParameter,

  /* token 500 - 599 */
  kInvalidAkId = 500,     /* 阿里云账号ak id无效 */
  kInvalidAkSecret,       /* 阿里云账号ak secret无效 */
  kInvalidAppKey,         /* 项目appKey无效 */
  kInvalidDomain,         /* domain无效 */
  kInvalidAction,         /* action无效 */
  kInvalidServerVersion,  /* ServerVersion无效 */
  kInvalidServerResource, /* ServerResource无效 */
  kInvalidRegionId,       /* RegionId无效 */
  kClientRequestFaild,    /* 账号客户端请求失败 */
  kInvalidUrlCmd,         /* 无效的url */
  kInvalidTokenCmd,       /* 无效的token指令 */
  kTokenConnectFailed,    /* 链接token服务器失败 */
  kTokenResponseError,    /* token返回失败 */

  kMaxConvRetCode = 999,
};

}  // namespace convsdk

#endif  // CONVSDK_CONV_CODE_H
