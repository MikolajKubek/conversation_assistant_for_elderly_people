#include <cstddef>
#include <string>
#include <SDL2/SDL.h>

class AsrClient {
public:
  virtual std::string transcribe(Uint8* buffer, size_t buffer_len) = 0;
};
