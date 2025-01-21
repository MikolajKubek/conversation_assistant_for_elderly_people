#include "prompt_processor/prompt_processor.hpp"
#include "inja/inja.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace inja;

std::string
prompt_processor::render_template(std::string user_prompt,
                                  std::vector<std::string> available_apis) {
  json data;
  data["user_prompt"] = user_prompt;
  data["available_apis"] = available_apis;
  Environment env;
  Template initial_interaction = env.parse_template(
      "../prompt_processor/templates/initial_interaction.txt");

  std::string rendered_template = env.render(initial_interaction, data);
  std::cout << "Rendered followup template: " << std::endl
            << rendered_template << std::endl;

  return rendered_template;
}

std::string prompt_processor::render_followup_template(
    std::string user_prompt, std::vector<std::pair<std::string, std::string>> followup_data,
    std::vector<std::string> available_apis) {
  json data;
  data["user_prompt"] = user_prompt;
    for (const auto& pair: followup_data) {
        data["followup_data"].push_back({{"name", pair.first}, {"value", pair.second}});

    }
  data["available_apis"] = available_apis;
  Environment env;
  Template followup_interaction = env.parse_template(
      "../prompt_processor/templates/followup_interaction_new.txt");

  std::string rendered_template = env.render(followup_interaction, data);
  std::cout << "Rendered followup template: " << std::endl
            << rendered_template << std::endl;

  return rendered_template;
}

std::string prompt_processor::render_followup_template(
    std::string user_prompt, std::string followup_data,
    std::vector<std::string> available_apis) {
  json data;
  data["user_prompt"] = user_prompt;
  data["followup_data"] = followup_data;
  data["available_apis"] = available_apis;
  Environment env;
  Template followup_interaction = env.parse_template(
      "../prompt_processor/templates/followup_interaction.txt");

  std::string rendered_template = env.render(followup_interaction, data);
  std::cout << "Rendered followup template: " << std::endl
            << rendered_template << std::endl;

  return rendered_template;
}
