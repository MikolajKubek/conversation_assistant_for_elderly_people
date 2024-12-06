#ifndef API_MANAGER_HPP
#define API_MANAGER_HPP

#include "interaction_db/interaction_db.hpp"
#include <chrono>
#include <curl/curl.h>
#include <curl/header.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using json = nlohmann::json;

class AssistantApi {
public:
  AssistantApi() {};
  ~AssistantApi() {};
  virtual std::string get_command() = 0;
  virtual void set_params(std::vector<std::string>) = 0;
  virtual std::string call() = 0;

private:
};

class ApiManager {
public:
  ApiManager() {};
  ~ApiManager() {};
  std::vector<std::string> get_apis() {
    std::vector<std::string> keys;
    for (auto it = m_api_registry.begin(); it != m_api_registry.end(); ++it) {
      keys.push_back(it->second->get_command());
    }

    return keys;
  }

  void register_api(std::string api_name,
                    std::unique_ptr<AssistantApi> assistant_api) {
    auto pair = m_api_registry.emplace(api_name, std::move(assistant_api));
    if (!pair.second) {
      std::cout << "unable to register api" << api_name << std::endl;
    }
  };
  std::pair<std::string, bool> handle_response(std::string model_response);
  static std::pair<std::string, std::vector<std::string>> read_api_params(std::string api_data);

private:
  std::map<std::string, std::unique_ptr<AssistantApi>> m_api_registry;
};

class RespondApi : public AssistantApi {
public:
  RespondApi(std::shared_ptr<InteractionDb> database_context) {
    const char *env_raw = std::getenv("MODEL_PATH");
    if (!env_raw) {
      std::cout << "Please provide MODEL_PATH variable" << std::endl;
      ;
      throw 345;
    }

    m_model_path = std::string(env_raw);
    m_database_context = database_context;
  };
  ~RespondApi() {};
  std::string command = "respond(\"TEXT\")";
  void set_params(std::vector<std::string> params) { m_params = params; }
  std::string call() {
    if (m_params.size() != 1) {
      std::cout << "WARN: respond api requires exactly one parameter"
                << std::endl;
      return "Something went wrong while calling " + command + " api";
    }

    m_database_context->insert("assistant", m_params[0]);

    say(m_params[0]);
    return "";
  }

  std::string get_command() { return command; }

private:
  std::vector<std::string> m_params;
  std::string m_model_path;
  std::shared_ptr<InteractionDb> m_database_context;
  void say(std::string text) {
    std::string command = "echo '";
    command.append(text);
    command.append("'");
    command.append(" | piper --model " + m_model_path +
                   " --output-raw | aplay -r 22050 -f S16_LE -t raw -");
    std::cout << command << std::endl;
    std::system(command.data());
  }
};

class TimeApi : public AssistantApi {
public:
  TimeApi() {};
  ~TimeApi() {};
  std::string command = "getTime()";
  void set_params(std::vector<std::string> params) {
    if (params.size() > 0) {
      std::cout << "expected no params but received: ";
      for (std::string param : params) {
        std::cout << param << ", ";
      }
      std::cout << std::endl;
    }
  }
  std::string call() {
    json response;
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    // ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    ss << std::put_time(std::localtime(&in_time_t), "%X");

    response["time"] = ss.str();

    return response.dump(2);
  }

  std::string get_command() { return command; }
};

class DateApi : public AssistantApi {
public:
  DateApi() {};
  ~DateApi() {};
  std::string command = "getDate()";
  void set_params(std::vector<std::string> params) {
    if (params.size() > 0) {
      std::cout << "expected no params but received: ";
      for (std::string param : params) {
        std::cout << param << ", ";
      }
      std::cout << std::endl;
    }
  }
  std::string call() {
    json response;
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%A %Y-%m-%d %X");

    response["dateTime"] = ss.str();

    return response.dump(2);
  }

  std::string get_command() { return command; }
};

inline size_t write_callback(char *contents, size_t size, size_t nmemb,
                             void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

class WeatherApi : public AssistantApi {
public:
  std::string command = "getWeather(\"LOCATION\")";
  WeatherApi(std::string api_key) {
    m_api_key = api_key;
    m_curl = curl_easy_init();
    if (!m_curl) {
      std::cout << "failed to initialize curl" << std::endl;
      throw 123;
    }
  };
  ~WeatherApi() { curl_easy_cleanup(m_curl); };
  void set_params(std::vector<std::string> params) { m_params = params; }
  std::string call() {
    if (m_params.size() < 1) {
      std::cout << "not enough params" << std::endl;
      return "Error: not enough params. The API signature is " + command;
    }
    std::string location = m_params[0].substr(0, m_params[0].length() - 1);
    std::string buffer;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &buffer);
    // std::string url = m_api_url + "?q=" + m_params[0] +
    // "&units=metric&lang=pl" + "&appid=" + m_api_key;
    std::string url =
        m_api_url + "?q=" + location + "&units=metric" + "&appid=" + m_api_key;
    curl_easy_setopt(m_curl, CURLOPT_URL, url.data());
    CURLcode res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
      return "The api call failed";
    }

    json response = json::parse(buffer);
    json response_filtered;
    response_filtered["weather_description"] =
        response.at("weather")[0].at("description");
    response_filtered["temp"] = response.at("main").at("temp");
    response_filtered["humidity"] = response.at("main").at("humidity");
    response_filtered["pressure"] = response.at("main").at("pressure");
    response_filtered["units"] = "metric";

    return response_filtered.dump(2);
  }

  std::string get_command() { return command; }

private:
  std::string m_api_key;
  std::string m_api_url = "api.openweathermap.org/data/2.5/weather";
  std::vector<std::string> m_params;
  CURL *m_curl;
};

class PreviousContextApi : public AssistantApi {
public:
  PreviousContextApi(std::shared_ptr<InteractionDb> db_context) {
    m_database_context = db_context;
  };
  ~PreviousContextApi() {};
  std::string command = "getPreviousContext()";
  void set_params(std::vector<std::string> params) {
    if (params.size() > 0) {
      std::cout << "expected no params but received: ";
      for (std::string param : params) {
        std::cout << param << ", ";
      }
      std::cout << std::endl;
    }
  }
  std::string call() {
    json response;
    response["previous_conversations"] = json::array();

    auto now = std::chrono::system_clock::now();
    auto five_minutes = std::chrono::minutes(5);
    auto time_threshold = now - five_minutes;
    auto time_threshold_time_t = std::chrono::system_clock::to_time_t(time_threshold);

    std::vector<InteractionDbRecord> records = m_database_context->select(time_threshold_time_t);
    for (InteractionDbRecord record: records) {
      std::string history_entry;
      if (record.sender == "user") {
        history_entry = "prompt: " + record.value;
      }
      else if (record.sender == "assistant") {
        history_entry = "assistant: " + record.value;
      }
      else {
        std::cout << "invalid record sender" << record.sender << std::endl;
      }

      response["previous_conversations"].push_back(history_entry);
    }
    return response.dump(2);
  }

  std::string get_command() { return command; }

private:
  std::shared_ptr<InteractionDb> m_database_context;
};

#endif // API_MANAGER
