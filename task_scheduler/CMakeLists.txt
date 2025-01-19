project(task_scheduler)

add_library(${PROJECT_NAME} task_scheduler.cpp)
add_library(task_scheduler_lib ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
