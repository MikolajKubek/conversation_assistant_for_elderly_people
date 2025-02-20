#include "api_manager/api_manager.hpp"
#include <cctype>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>
using json = nlohmann::json;

std::pair<std::string, std::vector<std::string>>
ApiManager::read_api_params(std::string api_data) {
  std::string api_name;
  std::vector<std::string> api_params;
  bool api_name_found = false;
  bool reading_string_param = false;
  int param_start_index = 0;
  char current_quote = '\0';

  std::string current_buffer;
  for (int i = 0; i < api_data.size(); i++) {
    char c = api_data[i];
    if (!api_name_found && c == '(') {
      api_name = current_buffer;
      current_buffer.clear();
      api_name_found = true;
      continue;
    }

    if (!reading_string_param && (c == '"' || c == '\'')) {
      current_quote = c;
      reading_string_param = true;
      continue;
    }

    if (reading_string_param && c == current_quote) {
      current_quote = '\0';
      reading_string_param = false;
      api_params.push_back(current_buffer);
      current_buffer.clear();
      continue;
    }

    if (reading_string_param && c == '\\') {
      if (i + 1 < api_data.size() && api_data[i+1] == current_quote) {
       current_buffer += api_data[i + 1];
       ++i; // Skip the escaped character
        continue;
      }
    }

    if (!reading_string_param && (c == ',' || c == ')')) {
      if (current_buffer.size() > 0) {
        api_params.push_back(current_buffer);
        current_buffer.clear();
      }
      continue;
    }

    if (!api_name_found || reading_string_param) {
      current_buffer += c;
    }
  }

  std::cout << "parsed response to api: " << api_name << " params: ";
  for (std::string s : api_params) {
    std::cout << s << ", ";
  }
  std::cout << std::endl;

  return std::make_pair(api_name, api_params);
}

HandleResponseResult
ApiManager::handle_response(std::string model_response) {
  bool is_final = false;
  std::string api_data;
  std::string data_learned = "";

  json response_json = json::parse(model_response);
  response_json.at("apiToCall").get_to(api_data);
  if (response_json.at("dataLearned").is_string()) {
    data_learned = response_json.at("dataLearned").get<std::string>();
  } else if (response_json.at("dataLearned").is_object() && !response_json.at("dataLearned").empty()) {
    response_json.at("dataLearned").dump();
  }

  std::cout << "got response: " << api_data << std::endl;

  auto api_data_serialized = read_api_params(api_data);
  std::string api_name = api_data_serialized.first;
  std::vector<std::string> api_params = api_data_serialized.second;

  auto assistant_iterator = m_api_registry.find(api_name);
  if (assistant_iterator == m_api_registry.end()) {
    std::cout << "Suitable api not available" << std::endl;
    return HandleResponseResult(api_name, "Requested api wasn't found", "", true);
  }

  assistant_iterator->second->set_params(api_params);
  std::string response = assistant_iterator->second->call();
  if (response.size() == 0) {
    is_final = true;
  }

  return HandleResponseResult(api_name, response, data_learned, is_final);
}
