cmake_minimum_required(VERSION 3.25)

project(altenpfleger-loop)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(interaction_db)
add_subdirectory(api_manager)
add_subdirectory(llm_interface)
add_subdirectory(prompt_processor)
add_subdirectory(task_scheduler)
# Tests
add_subdirectory(tests)

add_executable(main main.cpp)

target_link_libraries(main
	interaction_db_lib
	api_manager_lib
	llm_interface_lib
	prompt_processor_lib
	task_scheduler_lib
)
