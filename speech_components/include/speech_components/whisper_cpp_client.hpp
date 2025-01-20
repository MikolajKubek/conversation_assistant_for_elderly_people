#include <SDL_stdinc.h>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <speech_components/asr_clients.hpp>
#include <sstream>
#include <vector>
#include <whisper.h>

class WhisperClient : public AsrClient {
public:
  WhisperClient() {
    std::cout << "initialize whisper" << std::endl;
    struct whisper_context_params context_params =
        whisper_context_default_params();
    m_ctx = whisper_init_from_file_with_params(m_model_path.c_str(),
                                               context_params);
    if (!m_ctx) {
      std::cout << "ERROR: failed to load model" << std::endl;
      throw 222;
    }

    m_params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    m_params.language = "pl";
  }
  ~WhisperClient() { whisper_free(m_ctx); }

  std::string transcribe(Uint8 *buffer, size_t buffer_len) override {
    std::cout << "here whisper" << std::endl;
    for (int i = 0; i < buffer_len; i++) {
      std::cout << buffer[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << "here whisper2" << std::endl;
    std::vector<float> converted_data(buffer_len);
    convert_input_data(buffer, buffer_len, converted_data);
    std::cout << "here whisper3" << std::endl;

    if (!whisper_full(m_ctx, m_params, converted_data.data(), buffer_len)) {
      std::cout << "ERROR: failed to process audio" << std::endl;
      throw 333;
    }
    std::cout << "here whisper4" << std::endl;

    int num_segments = whisper_full_n_segments(m_ctx);

    std::stringstream result;

    for (int i = 0; i < num_segments; i++) {
      const char *text = whisper_full_get_segment_text(m_ctx, i);
      std::string segment_text(text);

      if (!isalpha(segment_text[0])) {
        std::cout << "WARN: segment doesn't start with a letter, segment: "
                  << segment_text << std::endl;
        continue;
      }
      std::cout << "segment text: " << segment_text << std::endl;
      result << segment_text;
    }

    return result.str();
  }

private:
  std::string m_model_path =
      "/home/mikolaj/cpp/altenpfleger.ai/deps/whisper.cpp/models/ggml-base.bin";
  std::string m_lang = "pl";
  whisper_context *m_ctx;
  whisper_full_params m_params;

  int convert_input_data(Uint8 *buffer, size_t buffer_len,
                         std::vector<float> &converted_data) {
    converted_data.resize(buffer_len);
    converted_data.clear();

    for (int i = 0; i < buffer_len; i++) {
      Sint16 sample = reinterpret_cast<Sint16 *>(buffer)[i];
      converted_data[i] =
          static_cast<float>(sample) / 32768.0f; // divide by 2^15 to normalize
    }

    return EXIT_SUCCESS;
  }
};
