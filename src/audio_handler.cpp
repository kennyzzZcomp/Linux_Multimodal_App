#include "audio_handler.h"
#include "conversation_handler.h"


using namespace convsdk;

// Save incoming binary payload to local files for inspection/playback.
// Writes both a per-chunk file and appends to a session-total file under `tmp/`.
void SaveBinaryEventToFile(ConvEvent* event) {
    if (!event) return;
    int size = event->GetBinaryDataSize();
    if (size <= 0) return;

    std::vector<unsigned char> data = event->GetBinaryData();

    // Ensure output dir exists
    const char* out_dir = "tmp";
    struct stat st = {0};
    if (stat(out_dir, &st) == -1) {
        mkdir(out_dir, 0755);
    }

    // Build filenames: per-chunk and total
    static std::atomic<int> chunk_counter{0};
    std::time_t t = std::time(nullptr);
    std::ostringstream oss;
    oss << out_dir << "/binary_" << event->GetSessionId() << "_" << t << "_" << chunk_counter++ << ".pcm";
    std::string chunk_path = oss.str();

    std::ofstream ofs(chunk_path, std::ios::binary | std::ios::out);
    if (ofs.is_open()) {
        ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
        ofs.close();
        std::cout << "Saved binary chunk to " << chunk_path << " (" << data.size() << " bytes)" << std::endl;
    } else {
        std::cerr << "Failed to open " << chunk_path << " for writing" << std::endl;
    }

    // Append to session total file
    std::ostringstream totaloss;
    totaloss << out_dir << "/binary_" << event->GetSessionId() << "_total.pcm";
    std::string total_path = totaloss.str();
    std::ofstream tofs(total_path, std::ios::binary | std::ios::app);
    if (tofs.is_open()) {
        tofs.write(reinterpret_cast<const char*>(data.data()), data.size());
        tofs.close();
    } else {
        std::cerr << "Failed to open " << total_path << " for appending" << std::endl;
    }
}

bool SendAudioFile(Conversation* conversation,
                   const std::string& file_path,
                   const std::string& audio_format,
                   int sample_rate,
                   size_t chunk_size,
                   bool skip_wav_header) {
    if (!conversation) return false;
    // Open the audio file
    std::ifstream fs(file_path, std::ios::binary);
    if (!fs.is_open()) {
        std::cerr << "SendAudioFile: failed to open " << file_path << std::endl;
        return false;
    }

    // If PCM and file is WAV, optionally skip RIFF header (44 bytes typical)
    if (skip_wav_header && audio_format == "pcm") {
        // Peek first 12 bytes to detect RIFF/WAVE
        char header[12] = {0};
        fs.read(header, 12);
        if (fs.gcount() == 12 &&
            header[0] == 'R' && header[1] == 'I' && header[2] == 'F' && header[3] == 'F' &&
            header[8] == 'W' && header[9] == 'A' && header[10] == 'V' && header[11] == 'E') {
            // typical WAV header length is 44 bytes; move to 44
            fs.seekg(44, std::ios::beg);
        } else {
            // not a WAV header -> rewind to file start
            fs.clear();
            fs.seekg(0, std::ios::beg);
        }
    }

    std::vector<uint8_t> buf(chunk_size);
    while (!fs.eof()) {
        fs.read(reinterpret_cast<char*>(buf.data()), buf.size());
        std::streamsize n = fs.gcount();
        if (n <= 0) break;

        // Send actual read length (do not always send fixed chunk_size)
        int ret_send = conversation->SendAudioData(buf.data(), static_cast<size_t>(n));
        if (ret_send != kSuccess) {
            std::cerr << "SendAudioFile: SendAudioData returned " << ret_send << " for " << n << " bytes" << std::endl;
        }

        // Sleep to simulate real-time streaming.
        if (audio_format == "pcm") {
            // PCM: bytes per second = sample_rate * channels(1) * bytes_per_sample(2)
            int bytes_per_second = sample_rate * 1 * 2;
            if (bytes_per_second > 0) {
                int duration_ms = static_cast<int>(n * 1000LL / bytes_per_second);
                if (duration_ms < 5) duration_ms = 5;
                std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        } else {
            // For encoded formats (e.g. opus) use a small fixed sleep
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    fs.close();
    return true;
}