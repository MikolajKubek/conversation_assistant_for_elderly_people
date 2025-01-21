#include <api_manager/api_manager.hpp>

class NewsApi : public AssistantApi {
public:
  std::string command = "getNews(\"TOPIC_ENG\")";
  NewsApi(std::string api_key) {
    m_api_key = api_key;
    m_curl = curl_easy_init();
    if (!m_curl) {
      std::cout << "failed to initialize curl" << std::endl;
      throw 123;
    }
  };
  ~NewsApi() { curl_easy_cleanup(m_curl); };

  void set_params(std::vector<std::string> params) { m_params = params; }

  std::string call() {
    std::string buffer;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &buffer);
    std::string url = build_url(m_params[0], "2025-01-20");
    curl_easy_setopt(m_curl, CURLOPT_URL, url.data());
    curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "Dark Secret Ninja/1.0");
    CURLcode res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
      return "The api call failed";
    }

    json response = json::parse(buffer);
    json response_filtered;
    if (!response.contains("articles") || !response.contains("totalResults")) {
      return std::string("Couldn't fetch articles for query ").append(m_params[0]);
    }

    response_filtered["received_news"] = json::array();
    int num_articles = response["totalResults"].get<int>();
    num_articles = num_articles > 10? 10 : num_articles;
    for (int i = 0; i < num_articles; i++) {
      json news_item;
      news_item["description"] = response.at("articles")[i].at("description");
      news_item["content"] = response.at("articles")[i].at("content");
      response_filtered["received_news"].push_back(news_item);
    }

    return response_filtered.dump(2);
  }

  std::string get_command() { return command; }

private:
  std::string m_api_key;
  // std::string m_api_url =
  std::string m_api_url = "https://newsapi.org/v2/everything";
  std::vector<std::string> m_params;
  CURL *m_curl;

  std::string replace_whitespaces(const std::string &input) {
    std::string result;
    for (char ch : input) {
      if (ch == ' ') {
        result += "%20";
      } else {
        result += ch;
      }
    }
    return result;
  }

  std::string build_url(std::string query, std::string date_from) {
    std::string base_url(m_api_url);
    base_url.append("?q=")
        .append(replace_whitespaces(query))
        .append("&from=")
        .append(date_from)
        .append("&lang=en")
        .append("&sortBy=popularity&apiKey=")
        .append(m_api_key);
    return base_url;
  }
};
