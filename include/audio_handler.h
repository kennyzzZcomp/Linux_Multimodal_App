#pragma once
#include <cstddef>
#include <string>
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
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <ctime>

#include "conversation.h"

extern convsdk::Conversation* conversation;

bool SendAudioFile(convsdk::Conversation* conversation,
				   const std::string& file_path,
				   const std::string& audio_format,
				   int sample_rate,
				   std::size_t chunk_size,
				   bool skip_wav_header=false);

void SaveBinaryEventToFile(convsdk::ConvEvent* event);