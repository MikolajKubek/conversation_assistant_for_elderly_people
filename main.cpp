#include "api_manager/api_manager.hpp"
#include "interaction_db/interaction_db.hpp"
#include "llm_interface/llm_interface.hpp"
#include "prompt_processor/prompt_processor.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define MAX_INTERACTOINS 2

std::string loop_callback(std::string user_prompt,
                          std::shared_ptr<GptInterface> llm_interface,
                          std::shared_ptr<ApiManager> api_manager) {
  std::vector<std::string> available_apis = api_manager->get_apis();
  std::string user_prompt_wrapped =
      prompt_processor::render_template(user_prompt, available_apis);
  std::string model_response = llm_interface->send_request(user_prompt_wrapped);

  int interaction_counter = 0;
  auto processed_response = api_manager->handle_response(model_response);
  bool is_final = processed_response.second;
  while (!is_final && interaction_counter < MAX_INTERACTOINS) {
    interaction_counter++;
    user_prompt_wrapped = prompt_processor::render_followup_template(
        user_prompt, processed_response.first, available_apis);
    model_response = llm_interface->send_request(user_prompt_wrapped);
    processed_response = api_manager->handle_response(model_response);
    is_final = processed_response.second;
  }

  return model_response;
}

std::string get_env(std::string env_name) {
  const char *env_raw = std::getenv(env_name.c_str());
  if (!env_raw) {
    std::cout << "Please provide " << env_name << " variable" << std::endl;
    ;
    throw 345;
  }

  return std::string(env_raw);
}

int main() {
  std::string openai_key = get_env("OPENAI_KEY");
  std::string openweather_key = get_env("OPENWEATHER_KEY");

  // Initialize gpt interface
  GptInterface gptInterface(openai_key);
  std::shared_ptr<GptInterface> interface_ptr =
      std::make_shared<GptInterface>(gptInterface);

  // Initialize db context
  std::shared_ptr<InteractionDb> interaction_db =
      std::make_shared<InteractionDb>();
  interaction_db->insert("user", "Która jest godzina?");
  interaction_db->print_current_state();

  // Initialize API manager
  auto api_manager = std::make_shared<ApiManager>();
  std::unique_ptr<AssistantApi> respond_api = std::make_unique<RespondApi>();
  std::unique_ptr<AssistantApi> time_api = std::make_unique<TimeApi>();
  std::unique_ptr<AssistantApi> date_api = std::make_unique<DateApi>();
  std::unique_ptr<AssistantApi> weather_api =
      std::make_unique<WeatherApi>(openweather_key);
  api_manager->register_api("respond", std::move(respond_api));
  api_manager->register_api("getTime", std::move(time_api));
  api_manager->register_api("getDate", std::move(date_api));
  api_manager->register_api("getWeather", std::move(weather_api));

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
