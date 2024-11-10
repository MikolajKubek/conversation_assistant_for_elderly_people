#include "prompt_processor.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void prompt_processor::render_template() {
    json data;

    data["name"] = "world";
    std::cout << "Hello from prompt processor" << data["name"] << std::endl;
}
