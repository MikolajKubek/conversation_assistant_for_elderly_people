# Define the source files for interaction_db
interaction_db_sources = [
  'interaction_db.cpp',
]

interaction_db_includes = include_directories('.')

# Create a library or shared object for interaction_db
interaction_db_lib = shared_library(
  'interaction_db',
  sources: interaction_db_sources,
  dependencies: [sqlite3_dep],
  include_directories: interaction_db_includes,
  install: true,
)

# Declare the dependency to be used by other subprojects
interaction_db_dep = declare_dependency(
  link_with: interaction_db_lib,
  include_directories: interaction_db_includes,
)
