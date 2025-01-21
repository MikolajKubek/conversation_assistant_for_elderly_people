#ifndef WIKIPEDIA_API_HPP
#define WIKIPEDIA_API_HPP

#include "api_manager/api_manager.hpp"
#include "nlohmann/detail/exceptions.hpp"
#include <codecvt>
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <locale>
#include <map>
#include <regex>
#include <string>
#include <vector>

// Function to replace Polish characters with their ASCII equivalents
inline std::string normalizePolishCharacters(const std::string &input) {
  // Mapping Polish characters to their ASCII equivalents
  static const std::unordered_map<char16_t, char16_t> polishMap = {
      {u'ą', 'a'}, {u'ć', 'c'}, {u'ę', 'e'}, {u'ł', 'l'}, {u'ń', 'n'},
      {u'ó', 'o'}, {u'ś', 's'}, {u'ź', 'z'}, {u'ż', 'z'}, {u'Ą', 'A'},
      {u'Ć', 'C'}, {u'Ę', 'E'}, {u'Ł', 'L'}, {u'Ń', 'N'}, {u'Ó', 'O'},
      {u'Ś', 'S'}, {u'Ź', 'Z'}, {u'Ż', 'Z'}};

  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::wstring wide_string = converter.from_bytes(input);

  std::string output;
  for (char16_t c : wide_string) {
    // Replace Polish character if found in map, otherwise keep the original
    auto it = polishMap.find(c);
    output += (it != polishMap.end()) ? it->second : c;
    if (it != polishMap.end()) {
    }
  }
  return output;
}

// Function to URLify the string
inline std::string urlify(const std::string &input) {
  // Step 1: Normalize Polish characters
  // std::string normalized = normalizePolishCharacters(input);

  // Step 2: Convert to lowercase
  // std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                 // ::tolower);

  // Step 3: Replace spaces with hyphens
  std::string replaced =
      std::regex_replace(input, std::regex("\\s+"), "%20");

  // Step 4: Remove non-alphanumeric characters except hyphens
  //replaced = std::regex_replace(replaced, std::regex("[^a-z0-9-]"), "");

  return replaced;
}

inline std::string
build_url(std::string base_url,
          std::map<std::string, std::string> request_params) {
  int params_size = request_params.size();
  if (params_size > 0) {
    base_url.append("?");
  }
  for (const auto &[key, value] : request_params) {
    params_size--;
    base_url.append(urlify(key)).append("=").append(urlify(value));
    if (params_size > 0) {
      base_url.append("&");
    }
  }

  std::cout << base_url << std::endl;
  return base_url;
}

class WikipediaApi : public AssistantApi {
public:
  std::string command = "getWikipediaArticle(\"TOPIC\")";
  WikipediaApi() {
    m_curl = curl_easy_init();
    if (!m_curl) {
      std::cerr << "failed to initialize curl" << std::endl;
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
      std::cerr << "not enough params" << std::endl;
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
    // TODO: replace spaces with %20
    std::string url =
        build_url(m_wikidata_search_entities_url, search_entities_params);

    curl_easy_setopt(m_curl, CURLOPT_URL, url.data());
    CURLcode res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
      return "The wikidata search entities api call failed";
    }
    std::cerr << "succesfully received first wikidata response" << std::endl;
    if (buffer.empty()) {
      return "Error: received empty response from wikidata";
    }

    json search_entity_response;
    try {
      search_entity_response = json::parse(buffer);
    } catch (nlohmann::detail::parse_error) {
      std::cerr << "unable to parse the API response: " << std::endl
                << buffer << std::endl;
      return "Error: unable to parse API response";
    }

    std::cerr << "successfully parsed the response" << std::endl;

    if (!search_entity_response.contains("search")) {
      std::cerr << "unable to find the search field in response json"
                << std::endl;
    }

    if (search_entity_response.at("search").size() == 0) {
      std::cerr << "searching for " << search_term
                << " failed, no article found" << std::endl;
      return "Error: No article found for " + search_term;
    }

    std::string wikidata_id =
        search_entity_response.at("search").at(0).at("id");

    // get pl wikipedia article title associated with wikidata id
    std::map<std::string, std::string> wikidata_get_entity_props{
        {"action", "wbgetentities"},
        {"format", "json"},
        {"ids", wikidata_id},
        {"languages", "pl"},
    };
    url = build_url(m_wikidata_search_entities_url, wikidata_get_entity_props);

    buffer.erase();
    curl_easy_setopt(m_curl, CURLOPT_URL, url.data());
    res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
      return "The wikidata get entity call failed";
    }
    std::cerr << "succesfully received second wikidata response" << std::endl;

    json wikidata_response;
    try {
      wikidata_response = json::parse(buffer);
    } catch (nlohmann::detail::parse_error) {
      std::cerr << "unable to parse the API response: " << std::endl
                << buffer << std::endl;
      return "Error: unable to parse API response";
    }

    std::cerr << "succesfully parsed second response" << std::endl;

    json sitelinks =
        wikidata_response.at("entities").at(wikidata_id).at("sitelinks");
    std::string article_title;
    if (sitelinks.contains(m_language + "wiki")) {
      article_title = sitelinks.at(m_language + "wiki").at("title");
    } else if (sitelinks.contains("enwiki")) {
      article_title = sitelinks.at("enwiki").at("title");
    } else {
      return "Error: no article found in neither " + m_language + " nor en";
    }

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

    buffer.erase();
    curl_easy_setopt(m_curl, CURLOPT_URL, url.data());
    res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
      std::cerr << res << std::endl;
      return "The wikipedia api call failed";
    }
    std::cerr << "succesfully received wikipedia response" << std::endl;

    json wikipedia_response;
    try {
      wikipedia_response = json::parse(buffer);
    } catch (nlohmann::detail::parse_error) {
      std::cerr << "unable to parse API response: " << std::endl
                << buffer << std::endl;
      return "Error: unable to parse API response";
    }
    json pages = wikipedia_response.at("query").at("pages");
    for (auto it = pages.begin(); it != pages.end(); ++it) {
      if (it.key() == "-1") {
        std::cerr << "Error: article " << article_title << " not found"
                  << std::endl;
        return "The article of given title wasn't found";
      } else {
        json response_filtered;
        response_filtered[article_title] = it.value().at("extract");
        return response_filtered.dump(2);
      }
    }

    return response.dump(2);
  }

  std::string get_command() override { return command; }

private:
  std::string m_language = "en";
  bool m_brief_response = true;
  CURL *m_curl;
  std::vector<std::string> m_params;
  std::string m_wikidata_search_entities_url =
      "https://www.wikidata.org/w/api.php";
  std::string m_wikipedia_url =
      "https://" + m_language + ".wikipedia.org/w/api.php";
};

#endif // WIKIPEDIA_API
