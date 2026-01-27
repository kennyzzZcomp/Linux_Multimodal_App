#pragma once

#include <cstddef>
#include <string>
#include <atomic>

#include "conversation.h"

// These globals are owned by main.cpp today.
extern std::string g_log_level;
extern std::string g_mode;
extern convsdk::Conversation* conversation;
extern std::atomic<bool> can_send_audio;

// Helpers implemented in main.cpp but used by callbacks.


// Conversation SDK callbacks + init params builder.
void onMessage(convsdk::ConvEvent* event, void* param);
void onEtMessage(convsdk::ConvLogLevel level, const char* log, void* user_data);
std::string gen_init_params();