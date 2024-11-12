#include "prompt_processor.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <inja.hpp>

using json = nlohmann::json;
using namespace inja;

std::string prompt_processor::render_template(std::string user_prompt) {
    json data;
    data["user_prompt"] = user_prompt;
    Environment env;
    Template initial_interaction = env.parse_template("../prompt_processor/templates/initial_interaction.txt");

    return env.render(initial_interaction, data);
}
