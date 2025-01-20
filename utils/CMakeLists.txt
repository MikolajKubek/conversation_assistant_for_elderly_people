project(utils)

include(FetchContent)
find_package(SDL2 REQUIRED)

add_library(${PROJECT_NAME} sdl_client.cpp)
add_library(utils_lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
