#ifndef LLM_INTERFACE_HPP
#define LLM_INTERFACE_HPP
#include <curl/curl.h>
#include <iostream>
#include <string>

class LlmInterface {
    public:
        LlmInterface(std::string api_key){}
        ~LlmInterface(){}
        virtual std::string send_request(std::string request_body) = 0;
    private:
        std::string m_api_key;
};


class GptInterface : LlmInterface {
    public:
        GptInterface(std::string api_key) : LlmInterface(api_key) {
            //curl_global_init(CURL_GLOBAL_DEFAULT);
            m_api_key = api_key;
            m_curl = curl_easy_init();
            if (!m_curl) {
                std::cout << "failed to initialize curl" << std::endl; 
                throw 123;
            }
            curl_easy_setopt(m_curl, CURLOPT_URL, m_api_url.data());
            curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);

        }

        std::string send_request(std::string request_body) override;

        ~GptInterface() {
            curl_easy_cleanup(m_curl);
            //curl_global_cleanup();
        }

    private:
        CURL *m_curl;
        std::string m_api_key;
        //std::string m_api_url = "https://jsonplaceholder.typicode.com/todos/1";
        std::string m_api_url = "https://api.openai.com/v1/chat/completions";
};

#endif //LLM_INTERFACE_HPP
