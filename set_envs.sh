#!/bin/bash

OPENAI_KEY_FILE="openai.key"
if test -f "$OPENAI_KEY_FILE"; then
    export OPENAI_KEY=$(cat "$OPENAI_KEY_FILE")
else
    echo "$OPENAI_KEY_FILE not found"
fi

OPENWEATHER_KEY_FILE="openweather.key"
if test -f "$OPENWEATHER_KEY_FILE"; then
    export OPENWEATHER_KEY=$(cat "$OPENWEATHER_KEY_FILE")
else
    echo "$OPENWEATHER_KEY_FILE not found"
fi

MODEL_PATH="$HOME/cpp/altenpfleger.ai/deps/piper/models/pl_PL-gosia-medium.onnx"
if test -f "$MODEL_PATH"; then
    export MODEL_PATH="$MODEL_PATH"
else
    echo "$MODEL_PATH doesn't exist"
fi
