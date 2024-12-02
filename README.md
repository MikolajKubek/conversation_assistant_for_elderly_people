# LLM-based conversation assistant

## Prerequisites
### Piper
### Meson
### OpenAI API

## Installation
1. Setup meson builddir
```console
meson setup builddir
```
    * In order to use the clangd with this project, you may want to link the `compile_commands.json` from builddir to project root

2. Export OPENAI_KEY env variable
```console
export OPENAI_KEY="open_ai_key_value_12345678910"
```

3. Compile the project with meson
```console
cd builddir
meson compile
```

4. Execute the binary
```console
./main
```
