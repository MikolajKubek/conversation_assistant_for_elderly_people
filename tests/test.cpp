#include "api_manager/wikipedia_api.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using Catch::Matchers::StartsWith;
TEST_CASE("Call wikipedia api with empty params", "[api_manager]") {
  WikipediaApi wikipedia_api;

  REQUIRE(wikipedia_api.get_command() == "getWikipediaArticle(TOPIC)");

  std::vector<std::string> empty_params;
  wikipedia_api.set_params(empty_params);
  REQUIRE(wikipedia_api.call() == "Error: not enough params. The API signature "
                                  "is getWikipediaArticle(TOPIC)");
}

TEST_CASE("Call wikipedia api with one word param", "[wikipedia_api]") {
  WikipediaApi wikipedia_api;

  std::vector<std::string> params{"Python)"};
  wikipedia_api.set_params(params);
  REQUIRE_THAT(wikipedia_api.call(),
               StartsWith("{\n  \"Python\": \"Python – język programowania "
                          "wysokiego poziomu ogólnego"));
}
