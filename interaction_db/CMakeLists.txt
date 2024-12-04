project(interaction_db)

find_package(SQLite3 REQUIRED)

add_library(${PROJECT_NAME} interaction_db.cpp)
add_library(interaction_db_lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC ${SQLite3_LIBRARY})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

