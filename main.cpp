//#include "interaction_loop/interaction_loop.hpp"
#include "llm_interface/llm_interface.hpp"
#include "prompt_processor/prompt_processor.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <string>

std::string loop_callback(std::string user_prompt, std::shared_ptr<GptInterface> llm_interface) {
    // prompt_processor merge prompt with initial template
    // prompt_processor::prepare_prompt(user_prompt)
    prompt_processor::render_template();
    return llm_interface->send_request(user_prompt);
}

void main_loop(std::function<std::string(std::string, std::unique_ptr<LlmInterface>)> callback, std::unique_ptr<LlmInterface> llmInterface) {
    return;
}

int main() {
    const std::string openai_key = std::getenv("OPENAI_KEY");
    if (openai_key.empty()) {
        std::cout << "Please provide open ai api key";
        return -1;
    }

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

