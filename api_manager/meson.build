# Define the source files for api_manager
api_manager_sources = [
  'api_manager.cpp',
]

# Create the library or executable
api_manager_lib = library(
  'api_manager',
  sources: api_manager_sources,
  link_with: [interaction_db_lib],
  dependencies: [curl_dep, json_dep, sqlite3_dep],
  include_directories: [interaction_db_includes, include_directories('.')],
)
