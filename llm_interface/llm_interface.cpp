#include "llm_interface.hpp"
#include <cstdio>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <curl/header.h>
#include <string>

using json = nlohmann::json;


size_t write_callback(char *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string GptInterface::send_request(std::string request_body) {
    json gpt_request_json;
    gpt_request_json["model"] = "gpt-4o-mini";
    gpt_request_json["messages"] = "[{\"role\": \"user\", \"content\": \"Say this is a test!\"}]";
    gpt_request_json["temperature"] = 0.7;
    std::cout << gpt_request_json.dump() << std::endl;

    std::string text("{ \"model\": \"gpt-4o-mini\", \"messages\": [{\"role\": \"user\", \"content\": \"Say this is a test!\"}], \"temperature\": 0.7 }");

    //curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, text.data());


    // set request headers
    struct curl_slist *list = NULL;
    std::string auth_header("Authorization: Bearer ");
    list = curl_slist_append(list, (auth_header.append(m_api_key)).data());
    list = curl_slist_append(list, "Content-Type: application/json");
    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, list);

    // save response to a buffer 
    std::string buffer; 
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &buffer);

    // send the request
    CURLcode res = curl_easy_perform(m_curl);
    curl_slist_free_all(list); /* free the list */

    if (res != CURLE_OK) {
        fprintf(stderr, "send request failed with %s\n", curl_easy_strerror(res));
        return "Sorry, there was some error while communicating with the server";
    }

    //json data = json::parse(buffer);
    //std::cout << data << std::endl;

    return buffer;
}

