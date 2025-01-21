#ifndef SDL_CLIENT_HPP
#define SDL_CLIENT_HPP
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_error.h>
#include <SDL_stdinc.h>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <thread>

#define DESIRED_FREQUENCY 16000
#define DESIRED_FORMAT AUDIO_S16
#define DESIRED_NUM_CHANNELS 1
#define DESIRED_NUM_SAMPLES 4096

// Maximum recording time
const int MAX_RECORDING_SECONDS = 60;

// Maximum recording time plus padding
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1;

#define DEFAULT_RECORDING_DEVICE_ID 0

class SdlClient {
public:
  SdlClient() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      std::cout << "ERROR: failed to initialize SDL" << std::endl;
      throw 999;
    }

    SDL_AudioDeviceID recording_device_id = 0;

    SDL_AudioSpec desired_recording_spec;
    SDL_zero(desired_recording_spec);
    desired_recording_spec.freq = DESIRED_FREQUENCY;
    desired_recording_spec.format = DESIRED_FORMAT;
    desired_recording_spec.channels = DESIRED_NUM_CHANNELS;
    desired_recording_spec.samples = DESIRED_NUM_SAMPLES;
    desired_recording_spec.callback = SdlClient::audio_recording_callback;
    desired_recording_spec.userdata = this;

    int num_recording_devices = SDL_GetNumAudioDevices(SDL_TRUE);

    if (num_recording_devices < 1) {
      std::cout << "ERROR: unable to get audio capure device, "
                << SDL_GetError() << std::endl;
      throw 888;
    } else if (num_recording_devices > 1) {
      std::cout << "INFO: more than one recording device discovered, "
                   "proceeding with first one"
                << std::endl;
      for (int i = 0; i < num_recording_devices; i++) {
        const char *deviceName = SDL_GetAudioDeviceName(i, SDL_TRUE);
        printf("%d - %s\n", i, deviceName);
      }
    }

    m_recording_device_id = SDL_OpenAudioDevice(
        SDL_GetAudioDeviceName(DEFAULT_RECORDING_DEVICE_ID, SDL_TRUE), SDL_TRUE,
        &desired_recording_spec, &m_recording_device_spec,
        SDL_AUDIO_ALLOW_FORMAT_CHANGE);

    if (m_recording_device_id == 0) {
      std::cout << "ERROR: failed to open recording device, " << SDL_GetError()
                << std::endl;
      throw 888;
    }

    int bytes_per_sample =
        m_recording_device_spec.channels *
        (SDL_AUDIO_BITSIZE(m_recording_device_spec.format) / 8);
    int bytes_per_second = m_recording_device_spec.freq * bytes_per_sample;

    m_buffer_byte_size = RECORDING_BUFFER_SECONDS * bytes_per_second;
    m_buffer_byte_max_position = MAX_RECORDING_SECONDS * bytes_per_second;

    m_recording_buffer = new Uint8[m_buffer_byte_size];
  }

  int record(Uint8 *target_buffer, int &buffer_len) {
    std::cout << "INFO: m_recording_device_id " << m_recording_device_id
              << std::endl;
    std::cout << "INFO: m_buffer_byte_max_position "
              << m_buffer_byte_max_position << std::endl;

    if (!target_buffer) {
      std::cerr << "ERROR: target_buffer is not allocated!" << std::endl;
      return EXIT_FAILURE;
    }

    std::memset(m_recording_buffer, 0, m_buffer_byte_size);
    m_buffer_byte_position = 0;
    m_final_buffer_position = 0;

    std::cout << "Start recording. Press any key to interrupt" << std::endl;
    std::thread recording_stop(&SdlClient::listen_for_recording_interrupt,
                               this);

    SDL_PauseAudioDevice(m_recording_device_id, SDL_FALSE);

    bool recording_started = false;

    while (true) {
      SDL_LockAudioDevice(m_recording_device_id);

      if (m_buffer_byte_position > 0 && !recording_started) {
        recording_started = true;
        std::cout << "INFO: recording started " << std::endl;
      }

      if (m_buffer_byte_position > m_buffer_byte_max_position) {
        m_final_buffer_position = m_buffer_byte_max_position;
        SDL_PauseAudioDevice(m_recording_device_id, SDL_TRUE);
        SDL_UnlockAudioDevice(m_recording_device_id);
        break;
      }

      if (stop_recording.load()) {
        std::cout << "INFO: recording stopped" << std::endl;
        m_final_buffer_position = m_buffer_byte_position;
        SDL_PauseAudioDevice(m_recording_device_id, SDL_TRUE);
        SDL_UnlockAudioDevice(m_recording_device_id);
        break;
      }

      SDL_UnlockAudioDevice(m_recording_device_id);
      SDL_Delay(10);
    }

    SDL_UnlockAudioDevice(m_recording_device_id);

    recording_stop.join();

    std::cout << "INFO: finished recording, recorded "
              << m_final_buffer_position << "bytes" << std::endl;

    if (m_final_buffer_position == 0) {
      std::cerr << "ERROR: nothing was recorded" << std::endl;
      return EXIT_FAILURE;
    }

    std::memset(target_buffer, 0, m_buffer_byte_size);
    std::cout << "memory set" << std::endl;
    std::memcpy(target_buffer, m_recording_buffer, m_final_buffer_position);
    std::cout << "memory copied" << std::endl;
    buffer_len = m_final_buffer_position;

    return EXIT_SUCCESS;
  }

  ~SdlClient() { SDL_Quit(); }

  int read_wav_file(std::string file_path, Uint8 *wav_buffer[], Uint32 &wav_len,
                    int desired_sample_rate) {
    if (!std::filesystem::exists(file_path)) {
      std::cout << "ERROR: file doesn't exist: " << file_path << std::endl;
      return EXIT_FAILURE;
    }

    SDL_AudioSpec wav_spec;

    if (SDL_LoadWAV(file_path.c_str(), &wav_spec, wav_buffer, &wav_len) ==
        nullptr) {
      std::cout << "ERROR: failed to read wav file: " << file_path << std::endl
                << SDL_GetError() << std::endl;
      return EXIT_FAILURE;
    }

    if (wav_spec.freq != desired_sample_rate) {
      std::cout << "ERROR: received sample rate is different than desired "
                << wav_spec.freq << " != " << desired_sample_rate << std::endl;
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }

  void save_wav_file(const char *filename, Uint8 *buffer, int length) {
    std::cout << "save wav file" << std::endl;
    std::ofstream wav_file(filename, std::ios::binary);

    uint32_t chunkSize = 36 + length;
    uint16_t audioFormat = 1; // PCM
    uint16_t numChannels = m_recording_device_spec.channels;
    uint32_t sampleRate = m_recording_device_spec.freq;
    uint16_t bitsPerSample = SDL_AUDIO_BITSIZE(m_recording_device_spec.format);
    uint16_t blockAlign = numChannels * (bitsPerSample / 8);
    uint32_t byteRate = sampleRate * blockAlign;

    // RIFF header
    wav_file.write("RIFF", 4);
    wav_file.write(reinterpret_cast<const char *>(&chunkSize), 4);
    wav_file.write("WAVE", 4);

    // fmt subchunk
    wav_file.write("fmt ", 4);
    uint32_t subchunk1Size = 16;
    wav_file.write(reinterpret_cast<const char *>(&subchunk1Size), 4);
    wav_file.write(reinterpret_cast<const char *>(&audioFormat), 2);
    wav_file.write(reinterpret_cast<const char *>(&numChannels), 2);
    wav_file.write(reinterpret_cast<const char *>(&sampleRate), 4);
    wav_file.write(reinterpret_cast<const char *>(&byteRate), 4);
    wav_file.write(reinterpret_cast<const char *>(&blockAlign), 2);
    wav_file.write(reinterpret_cast<const char *>(&bitsPerSample), 2);

    // data subchunk
    wav_file.write("data", 4);
    wav_file.write(reinterpret_cast<const char *>(&length), 4);
    wav_file.write(reinterpret_cast<const char *>(buffer), length);

    wav_file.close();
    std::cout << "Saved WAV file: " << filename << std::endl;
  }

  int get_buffer_size() { return m_buffer_byte_size; }

private:
  Uint8 *m_recording_buffer = NULL;
  Uint32 m_buffer_byte_size = 0;
  Uint32 m_buffer_byte_position = 0;
  Uint32 m_buffer_byte_max_position = 0;
  SDL_AudioSpec m_recording_device_spec;
  SDL_AudioDeviceID m_recording_device_id;
  std::atomic<bool> stop_recording = false;

  // in case the recording ended earlier, m_final_buffer_position stores the
  // last index of recorded audio
  int m_final_buffer_position = 0;

  static void audio_recording_callback(void *userdata, Uint8 *stream, int len) {
    // std::cout << "callback " << len << std::endl;
    auto *sdl_client = static_cast<SdlClient *>(userdata);
    sdl_client->process_recording_audio(stream, len);
  }

  void process_recording_audio(Uint8 *stream, int len) {
    // std::cout << "process_recording_audio  " << len << std::endl;
    std::memcpy(&m_recording_buffer[m_buffer_byte_position], stream, len);

    m_buffer_byte_position += len;
  }

  void listen_for_recording_interrupt() {
    char x;
    std::cin >> x;
    stop_recording = true;
    std::cout << "Recording interrupted" << std::endl;
  }
};
#endif
