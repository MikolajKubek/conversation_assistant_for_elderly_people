#include "api_manager/api_manager.hpp"
#include "api_manager/wikipedia_api.hpp"
#include "speech_components/whisper_cpp_client.hpp"
#include "utils/sdl_client.hpp"
#include "SDL.h"
#include "catch2/matchers/catch_matchers.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <iostream>
#include <memory>

using Catch::Matchers::StartsWith;
TEST_CASE("Call wikipedia api with empty params", "[wikipedia_api]") {
  WikipediaApi wikipedia_api;

  REQUIRE(wikipedia_api.get_command() == "getWikipediaArticle(\"TOPIC\")");

  std::vector<std::string> empty_params;
  wikipedia_api.set_params(empty_params);
  REQUIRE(wikipedia_api.call() == "Error: not enough params. The API signature "
                                  "is getWikipediaArticle(\"TOPIC\")");
}

TEST_CASE("Call wikipedia api with one word param", "[wikipedia_api]") {
  WikipediaApi wikipedia_api;

  std::vector<std::string> params{"Python)"};
  wikipedia_api.set_params(params);
  REQUIRE_THAT(wikipedia_api.call(),
               StartsWith("{\n  \"Python\": \"Python – język programowania "
                          "wysokiego poziomu ogólnego"));
}

TEST_CASE("Call wikipedia api with two word param", "[wikipedia_api]") {
  WikipediaApi wikipedia_api;

  std::vector<std::string> params{"Bjarne Stroustrup)"};
  wikipedia_api.set_params(params);
  REQUIRE_THAT(wikipedia_api.call(),
               StartsWith("{\n  \"Bjarne Stroustrup\": \"Bjarne Stroustrup"));
}

TEST_CASE("Normalize polish characters", "[utils]") {
  REQUIRE(normalizePolishCharacters("Zażółć gęślą jaźń") == "Zazolc gesla jazn");
  REQUIRE(normalizePolishCharacters("abcdefghijklmnopqrstuvwxyz") == "abcdefghijklmnopqrstuvwxyz");
  REQUIRE(normalizePolishCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  REQUIRE(normalizePolishCharacters("1234567890") == "1234567890");
  REQUIRE(normalizePolishCharacters("!@#$%^&*()") == "!@#$%^&*()");
  REQUIRE(normalizePolishCharacters("`~[{}];:\"',<.>/?\\|") == "`~[{}];:\"',<.>/?\\|");
}

TEST_CASE("Read api params", "[utils]") {
  auto result = ApiManager::read_api_params("getWikipediaArticle(\"Nairobi\")");
  REQUIRE(result.first == "getWikipediaArticle");
  REQUIRE(result.second.size() == 1);
  REQUIRE(result.second[0] == "Nairobi");

  result = ApiManager::read_api_params("respond(\"Dzień dobry\")");
  REQUIRE(result.first == "respond");
  REQUIRE(result.second.size() == 1);
  REQUIRE(result.second[0] == "Dzień dobry");

  result = ApiManager::read_api_params("imaginaryApi(\"Dzień dobry\", \"Gdyby był dobry, to byłbym na rybach\")");
  REQUIRE(result.first == "imaginaryApi");
  REQUIRE(result.second.size() == 2);
  REQUIRE(result.second[0] == "Dzień dobry");
  REQUIRE(result.second[1] == "Gdyby był dobry, to byłbym na rybach");

  result = ApiManager::read_api_params("getWikipediaArticle('Nairobi')");
  REQUIRE(result.first == "getWikipediaArticle");
  REQUIRE(result.second.size() == 0);
}

TEST_CASE("Use whisper to transcribe audio", "[speech_components]") {
  auto whisper_client = WhisperClient();

  std::string wav_file_path = "/home/mikolaj/cpp/sdl_recording_test/build/recording.wav";
  Uint8* buffer;
  Uint32 buffer_len;

  auto sdl_client = SdlClient();
  sdl_client.read_wav_file(wav_file_path, &buffer, buffer_len, 16000);

  auto result = whisper_client.transcribe(buffer, buffer_len);

  REQUIRE(result == "Która jest godzina?");
}
