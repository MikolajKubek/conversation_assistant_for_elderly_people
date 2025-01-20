project(speech_components)

include(FetchContent)
find_package(SDL2 REQUIRED)
FetchContent_Declare(whisper URL "https://github.com/ggerganov/whisper.cpp/archive/refs/tags/v1.7.2.tar.gz")
FetchContent_MakeAvailable(whisper)

add_library(${PROJECT_NAME} speech_components.cpp)
add_library(speech_components_lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES} whisper)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)


