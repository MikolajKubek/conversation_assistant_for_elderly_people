//#include "interaction_loop/interaction_loop.hpp"
#include "llm_interface/llm_interface.hpp"
#include "prompt_processor/prompt_processor.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

void say(std::string text) {
    std::string command = "echo '";
    command.append(text);
    command.append("'");
    command.append(" | piper --model /home/mikolaj/cpp/altenpfleger.ai/deps/piper/models/pl_PL-gosia-medium.onnx --output-raw | aplay -r 22050 -f S16_LE -t raw -");
    std::system(command.data());
}

std::string loop_callback(std::string user_prompt, std::shared_ptr<GptInterface> llm_interface) {
    // prompt_processor merge prompt with initial template
    // prompt_processor::prepare_prompt(user_prompt)
    prompt_processor::render_template();
    std::string model_response = llm_interface->send_request(user_prompt);
    say(model_response);
    return model_response;
}

int main() {
    const char *openai_key_raw = std::getenv("OPENAI_KEY");
    if (!openai_key_raw) {
        std::cout << "Please provide open ai api key" << std::endl;;
        return -1;
    }
    std::string openai_key(openai_key_raw);

    GptInterface gptInterface(openai_key);
    std::shared_ptr<GptInterface> interface_ptr = std::make_shared<GptInterface>(gptInterface);

    std::cout << "Start interacting with the assistant:" << std::endl;

    std::string prompt;
    while (true) {
        std::getline(std::cin, prompt);

        if (prompt == "exit()") {
            break;
        }
        std::cout << loop_callback(prompt, interface_ptr) << std::endl;
    }

    return 0;
}
