cmake_minimum_required(VERSION 3.31)

project(main)

add_subdirectory(interaction_db)
add_subdirectory(api_manager)
add_subdirectory(llm_interface)
add_subdirectory(prompt_processor)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
	interaction_db_lib
	api_manager_lib
	llm_interface_lib
	prompt_processor_lib
)
