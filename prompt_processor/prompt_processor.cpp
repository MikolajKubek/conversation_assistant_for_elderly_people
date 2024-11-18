#include "prompt_processor.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <inja.hpp>

using json = nlohmann::json;
using namespace inja;

std::string prompt_processor::render_template(std::string user_prompt, std::vector<std::string> available_apis) {
    json data;
    data["user_prompt"] = user_prompt;
    data["available_apis"] = available_apis;
    Environment env;
    Template initial_interaction = env.parse_template("../prompt_processor/templates/initial_interaction.txt");

    std::string rendered_template = env.render(initial_interaction, data);
    std::cout << "Rendered followup template: " << std::endl << rendered_template << std::endl;

    return rendered_template;

}

std::string prompt_processor::render_followup_template(std::string user_prompt, std::string followup_data, std::vector<std::string> available_apis) {
    json data;
    data["user_prompt"] = user_prompt;
    data["followup_data"] = followup_data;
    data["available_apis"] = available_apis;
    Environment env;
    Template followup_interaction = env.parse_template("../prompt_processor/templates/followup_interaction.txt");

    std::string rendered_template = env.render(followup_interaction, data);
    std::cout << "Rendered followup template: " << std::endl << rendered_template << std::endl;

    return rendered_template;
}
