#ifndef API_MANAGER_HPP
#define API_MANAGER_HPP

#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class AssistantApi {
    public:
        AssistantApi(){};
        ~AssistantApi(){};
        virtual void set_params(std::vector<std::string>) = 0;
        virtual std::string call() = 0;
    private:
};

class ApiManager {
    public:
        ApiManager(){};
        ~ApiManager(){};
        void register_api(std::string api_name, std::unique_ptr<AssistantApi> assistant_api){
           auto pair = m_api_registry.emplace(api_name, std::move(assistant_api));
           if (!pair.second) {
               std::cout << "unable to register api" << api_name << std::endl;
           }
        };
        std::pair<std::string, bool> handle_response(std::string model_response);
    private:
        std::map<std::string, std::unique_ptr<AssistantApi>> m_api_registry;
};

class RespondApi: public AssistantApi {
    public:
        RespondApi(){};
        ~RespondApi(){};
        std::string command = "respond(TEXT)";
        void set_params(std::vector<std::string> params) { 
            m_params = params;
        }
        std::string call() {
            if (m_params.size() != 1) {
                std::cout << "WARN: respond api requires exactly one parameter" << std::endl;
                return "Something went wrong while calling " + command + " api";
            }

            say(m_params[0]);
            return "";
        }

    private:
        std::vector<std::string> m_params;
        void say(std::string text) {
            std::string command = "echo '";
            command.append(text);
            command.append("'");
            command.append(" | piper --model /home/mikolaj/cpp/altenpfleger.ai/deps/piper/models/pl_PL-gosia-medium.onnx --output-raw | aplay -r 22050 -f S16_LE -t raw -");
            std::system(command.data());
    }
};

class TimeApi: public AssistantApi {
    public:
        TimeApi(){};
        ~TimeApi(){};
        std::string command = "getTime()";
        void set_params(std::vector<std::string> params) {
            if (params.size() > 0) {
                std::cout << "expected no params but received: ";
                for (std::string param: params) {
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
            //ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
            ss << std::put_time(std::localtime(&in_time_t), "%X");

            response["time"] = ss.str();

            return response.dump(2);
        }
    private:

};

class DateApi: public AssistantApi {
    public:
        DateApi(){};
        ~DateApi(){};
        std::string command = "getDate()";
        void set_params(std::vector<std::string> params) {
            if (params.size() > 0) {
                std::cout << "expected no params but received: ";
                for (std::string param: params) {
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
    private:

};

#endif //API_MANAGER
