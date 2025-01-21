#ifndef SDL_CLIENT_HPP
#define SDL_CLIENT_HPP
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_stdinc.h>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ostream>

class SdlClient {
public:
  SdlClient() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      std::cout << "ERROR: failed to initialize SDL" << std::endl;
      throw 999;
    }
  }
  ~SdlClient() { SDL_Quit(); }
  int read_wav_file(std::string file_path, Uint8 *wav_buffer[], Uint32 &wav_len, int desired_sample_rate) {
    if (!std::filesystem::exists(file_path)) {
      std::cout << "ERROR: file doesn't exist: " << file_path << std::endl;
      return EXIT_FAILURE;
    }

    SDL_AudioSpec wav_spec;

    if (SDL_LoadWAV(file_path.c_str(), &wav_spec, wav_buffer, &wav_len) == nullptr) {
      std::cout << "ERROR: failed to read wav file: " << file_path << std::endl << SDL_GetError() << std::endl;
      return EXIT_FAILURE;
    }

    if (wav_spec.freq != desired_sample_rate) {
      std::cout << "ERROR: received sample rate is different than desired " << wav_spec.freq << " != " << desired_sample_rate << std::endl;
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }
};
#endif
