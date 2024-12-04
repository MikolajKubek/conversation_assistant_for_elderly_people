project(prompt_processor)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)
find_package(CURL REQUIRED)
add_library(${PROJECT_NAME} prompt_processor.cpp)
add_library(prompt_processor_lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json ${CURL_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
