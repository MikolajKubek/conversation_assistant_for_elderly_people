#ifndef WIKIPEDIA_API_HPP
#define WIKIPEDIA_API_HPP

#include "api_manager/api_manager.hpp"
#include "nlohmann/detail/exceptions.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <map>
#include <string>

inline std::string
build_url(std::string base_url,
          std::map<std::string, std::string> request_params) {
  int params_size = request_params.size();
  if (params_size > 0) {
    base_url.append("?");
  }
  for (const auto &[key, value] : request_params) {
    params_size--;
    base_url.append(key).append("=").append(value);
    if (params_size > 0) {
      base_url.append("&");
    }
  }

  std::cout << base_url << std::endl;
  return base_url;
}

class WikipediaApi : public AssistantApi {
public:
  std::string command = "getWikipediaArticle(TOPIC)";
  WikipediaApi() {
    m_curl = curl_easy_init();
    if (!m_curl) {
      std::cout << "failed to initialize curl" << std::endl;
      throw 123;
    }
  };
  ~WikipediaApi() { curl_easy_cleanup(m_curl); };
  void set_params(std::vector<std::string> params) override {
    m_params = params;
  };
  std::string call() override {
    json response;
    if (m_params.size() < 1) {
      std::cout << "not enough params" << std::endl;
      return "Error: not enough params. The API signature is " + command;
    }
    std::string search_term = m_params[0].substr(0, m_params[0].length() - 1);
    std::string buffer;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &buffer);

    // search entity by unprecise match - by calling a wikidata search api
    std::map<std::string, std::string> search_entities_params{
        {"action", "wbsearchentities"},
        {"format", "json"},
        {"search", search_term},
        {"language", m_language}};
    std::string url =
        build_url(m_wikidata_search_entities_url, search_entities_params);

    curl_easy_setopt(m_curl, CURLOPT_URL, url.data());
    CURLcode res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
      return "The wikidata search entities api call failed";
    }
    std::cout << "succesfully received first wikidata response" << std::endl;

    json search_entity_response;
    try {
      search_entity_response = json::parse(buffer);
    } catch (nlohmann::detail::parse_error) {
      std::cout << "unable to parse the API response: " << std::endl << buffer << std::endl;
      return "Error: unable to parse API response";
    }

    if (search_entity_response.at("search").size() == 0) {
      std::cout << "searching for " << search_term
                << " failed, no article found" << std::endl;
      return "Error: No article found for " + search_term;
    }

    std::string wikidata_id = response["search"][0]["id"];

    // get pl wikipedia article title associated with wikidata id
    url = m_wikidata_entity_url + "/" + wikidata_id + "?language=pl&format=json";

    curl_easy_setopt(m_curl, CURLOPT_URL, url.data());
    res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
      return "The wikidata get entity call failed";
    }
    std::cout << "succesfully received second wikidata response" << std::endl;

    json wikidata_response;
    try {
      wikidata_response = json::parse(buffer);
    } catch (nlohmann::detail::parse_error) {
      std::cout << "unable to parse the API response: " << std::endl << buffer << std::endl;
      return "Error: unable to parse API response";
    }

    std::string article_title =
        wikidata_response["entities"][wikidata_id]["sitelinks"]["plwiki"]["title"];

    // get the actual article intro
    // mediawiki api doc:
    // https://www.mediawiki.org/w/api.php?action=help&modules=query
    std::map<std::string, std::string> wikipedia_query_params{
        {"action", "query"},       {"prop", "extracts"},
        {"titles", article_title}, {"format", "json"},
        {"explaintext", "false"},
    };

    if (m_brief_response) {
      wikipedia_query_params.emplace("exintro", "");
    }

    url = build_url(m_wikipedia_url, wikipedia_query_params);

    curl_easy_setopt(m_curl, CURLOPT_URL, url.data());
    res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
      return "The wikipedia api call failed";
    }
    std::cout << "succesfully received wikipedia response" << std::endl;

    json wikipedia_response;
    try {
      wikipedia_response = json::parse(buffer);
    } catch (nlohmann::detail::parse_error) {
      std::cout << "unable to parse API response: " << std::endl << buffer << std::endl;
      return "Error: unable to parse API response";
    }
    json pages = wikipedia_response["query"]["pages"];
    for (auto it = pages.begin(); it != pages.end(); ++it) {
      if (it.key() == "-1") {
        std::cout << "Error: article " << article_title << " not found"
                  << std::endl;
        return "The article of given title wasn't found";
      } else {
        json response_filtered;
        response_filtered["title"] = article_title;
        response_filtered["description"] = it.value()["extract"];
      }
    }

    return response.dump(2);
  }

  std::string get_command() override { return command; }

private:
  std::string m_language = "pl";
  bool m_brief_response = true;
  CURL *m_curl;
  std::vector<std::string> m_params;
  std::string m_wikidata_search_entities_url =
      "https://www.wikidata.org/w/api.php";
  std::string m_wikidata_entity_url = "http://www.wikidata.org/entity";
  std::string m_wikipedia_url =
      "https://" + m_language + ".wikipedia.org/w/api.php";
};

#endif // WIKIPEDIA_API
