#include "api_manager.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <utility>
#include <vector>
using json = nlohmann::json;

std::pair<std::string, bool> ApiManager::handle_response(std::string model_response) {
    bool is_final = false;
    std::string api_data;
    std::string api_name;
    std::vector<std::string> api_params;

    json response_json = json::parse(model_response);
    response_json.at("apiToCall").get_to(api_data);

    std::cout << "got response: " << api_data << std::endl;

    bool api_name_found = false;
    int param_start_index = 0;
    // TODO: export to a separate function and unit test
    for (int i = 0; i < (int)api_data.size(); i++) {
        if (api_data[i] == '(' && !api_name_found) {
            api_name = api_data.substr(0, i);
            api_name_found = true;
            param_start_index = i + 1;
        }
        else if (!api_name_found) {
            // go through the string until api name is found
            continue;
        }
        // process parameters
        if (api_data[i] == ')') {
            if (i != (int)api_data.size() - 1) {
                std::cout << "Closing parenthesis found in the middle of the api call but should only occurr at the end" << std::endl;
            }
            if (i-1 > param_start_index) {
                api_params.push_back(api_data.substr(param_start_index, i-1));
            }
        }

        if (api_data[i] == ',' && i > 0 && api_data[i - 1] == '\'') {
            api_params.push_back(api_data.substr(param_start_index, i));
            param_start_index = i + 1;
        }
    }

    std::cout << "parsed response to api: " << api_name << " params: ";
    for (std::string s : api_params) {
        std::cout << s << ", ";
    }
    std::cout << std::endl;

    auto assistant_iterator = m_api_registry.find(api_name);
    if (assistant_iterator == m_api_registry.end()) {
        std::cout << "Suitable api not available" << std::endl;
        return std::make_pair("Requested api: " + api_name + "wasn't found", true);
    }

    assistant_iterator->second->set_params(api_params);
    std::string response = assistant_iterator->second->call();
    if (response.size() == 0) {
        is_final = true;
    }
    
    return std::make_pair(response, is_final);
}
