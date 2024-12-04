project(api_manager)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)
find_package(CURL REQUIRED)

add_library(${PROJECT_NAME} api_manager.cpp)
add_library(api_manager_lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json ${CURL_LIBRARIES} interaction_db_lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
