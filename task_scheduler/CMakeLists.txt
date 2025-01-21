project(task_scheduler)

include(FetchContent)
FetchContent_Declare(date
	GIT_REPOSITORY https://github.com/HowardHinnant/date.git
	GIT_TAG 5bdb7e6f31fac909c090a46dbd9fea27b6e609a4 # release v3.0.3
)
FetchContent_MakeAvailable(date)
FetchContent_Declare(libcron
	GIT_REPOSITORY https://github.com/PerMalmberg/libcron.git
	GIT_TAG 41f238ceb09d4179e7346d78584a0c978e5d0059 # release v1.3.2
)
FetchContent_MakeAvailable(libcron)
add_library(${PROJECT_NAME} task_scheduler.cpp)
add_library(task_scheduler_lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC date libcron)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
