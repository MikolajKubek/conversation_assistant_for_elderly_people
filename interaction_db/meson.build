sqlite3_dep = dependency('sqlite3')

interaction_db_src = ['interaction_db.cpp']

interaction_db_lib = library(
  'interaction_db',
  interaction_db_src,
  dependencies: [sqlite3_dep],
  include_directories: include_directories('.'),
  install: true
)

interaction_db_dep = declare_dependency(link_with: interaction_db_lib, include_directories: include_directories('.'))
