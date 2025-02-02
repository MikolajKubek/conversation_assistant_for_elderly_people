# LLM-based conversation assistant

## Prerequisites
### Piper
### OpenAI API

## Installation
1. Build CMake project
```console
mkdir build && cd build
cmake ..
make
```
- In order to use the clangd with this project, you may want to link the `compile_commands.json` from build dir to the project root
```console
ln -s build/compile_commands.json compile_commands.json
```

2. Export env variables using `set_envs.sh`
- OPENAI_KEY - API key for OpenAI API
- OPENWEATHER_KEY - API key for OpenWeather
- MODEL_PATH - model path for PIPER TTS

```console
source set_envs.sh
```
You may need to modify the script if env values need to be extracted in a way that's different from default.

3. Execute the project
```console
cd build
./main
```

4. On Raspberry pi, set the ESPEAK_DATA_PATH
```console
export ESPEAK_DATA_PATH=/usr/lib/aarch64-linux-gnu/espeak-ng-data
```

4. Execute the binary
```console
./main
```

## Demos
- Videos presenting interactions with the system
    - [Dyskusja o Warszawie; Lenovo y520; whisper ggml-tiny](https://youtu.be/GZsuQkUq65w)
    - [Powiedz mi coś ciekawego; Raspberry Pi; whisper ggml-tiny](https://youtu.be/BVK6LL-_ECM)
    - [Która jest godzina?](https://youtube.com/shorts/E2_d_ra0lQs?feature=share), ([version without cut](https://youtube.com/shorts/bExKkrKoZL8))
    - [Jak mam się dzisiaj ubrać?](https://youtube.com/shorts/Y4UXjJyt2A0?feature=share), ([version without cut](https://youtube.com/shorts/U-QJSh-cbgA?feature=share))
