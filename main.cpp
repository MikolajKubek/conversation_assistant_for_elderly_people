#include "api_manager/api_manager.hpp"
#include "llm_interface/llm_interface.hpp"
#include "prompt_processor/prompt_processor.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#define MAX_INTERACTOINS 2

void say(std::string text) {
    std::string command = "echo '";
    command.append(text);
    command.append("'");
    command.append(" | piper --model /home/mikolaj/cpp/altenpfleger.ai/deps/piper/models/pl_PL-gosia-medium.onnx --output-raw | aplay -r 22050 -f S16_LE -t raw -");
    std::system(command.data());
}

std::string loop_callback(std::string user_prompt, std::shared_ptr<GptInterface> llm_interface, std::shared_ptr<ApiManager> api_manager) {
    std::string user_prompt_wrapped = prompt_processor::render_template(user_prompt);
    std::string model_response = llm_interface->send_request(user_prompt_wrapped);

    int interaction_counter = 0;
    auto processed_response = api_manager->handle_response(model_response);
    bool is_final = processed_response.second;
    while (!is_final && interaction_counter < MAX_INTERACTOINS) {
        interaction_counter++;
        user_prompt_wrapped = prompt_processor::render_followup_template(user_prompt, processed_response.first);
        model_response = llm_interface->send_request(user_prompt_wrapped);
        processed_response = api_manager->handle_response(model_response);
        is_final = processed_response.second;
    }

    return model_response;
}

int main() {
    // Read OpenAI API key 
    const char *openai_key_raw = std::getenv("OPENAI_KEY");
    if (!openai_key_raw) {
        std::cout << "Please provide open ai api key" << std::endl;;
        return -1;
    }
    std::string openai_key(openai_key_raw);

    // Initialize gpt interface
    GptInterface gptInterface(openai_key);
    std::shared_ptr<GptInterface> interface_ptr = std::make_shared<GptInterface>(gptInterface);

    // Initialize API manager
    auto api_manager = std::make_shared<ApiManager>();
    std::unique_ptr<AssistantApi> respond_api = std::make_unique<RespondApi>();
    std::unique_ptr<AssistantApi> time_api = std::make_unique<TimeApi>();
    std::unique_ptr<AssistantApi> date_api = std::make_unique<DateApi>();
    api_manager->register_api("respond", std::move(respond_api));
    api_manager->register_api("getTime", std::move(time_api));
    api_manager->register_api("getDate", std::move(date_api));

    // Initialize interaction loop
    std::cout << "Start interacting with the assistant:" << std::endl;

    std::string prompt;
    while (true) {
        std::getline(std::cin, prompt);

        if (prompt == "exit()") {
            break;
        }
        std::cout << loop_callback(prompt, interface_ptr, api_manager) << std::endl;
    }

    return 0;
}
