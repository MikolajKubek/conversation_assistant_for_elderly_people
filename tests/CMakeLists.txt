project(tests)
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

# These tests can use the Catch2-provided main
add_executable(tests test.cpp)
set(ENV(TEST_DATA_DIR) ${PROJECT_SOURCE_DIR}/test_data)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain
	interaction_db_lib
	api_manager_lib
	llm_interface_lib
	prompt_processor_lib
	speech_components_lib
	utils_lib
)
