cmake_minimum_required(VERSION 3.10)
project(altenpfleger-loop)

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(prompt_processor)
include_directories(interaction_loop)

add_executable(main main.cpp)
target_sources(main PRIVATE interaction_loop/interaction_loop.cpp)
target_link_libraries(main PRIVATE PromptProcessor)
