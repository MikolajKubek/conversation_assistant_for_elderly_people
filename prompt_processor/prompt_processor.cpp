#include "prompt_processor.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <inja.hpp>

using json = nlohmann::json;

void prompt_processor::render_template() {
    json data;
    data["name"] = "world";
    std::cout << inja::render("Hello {{ name }}!", data) << std::endl;
}
