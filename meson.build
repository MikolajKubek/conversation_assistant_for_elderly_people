project('main_loop', 'cpp')

curl_dep = dependency('libcurl')
json_dep = dependency('nlohmann_json')
openssl_dep = dependency('openssl')
sqlite3_dep = dependency('sqlite3')

subdir('interaction_db')
subdir('api_manager')
subdir('llm_interface')
subdir('prompt_processor')

executable(
  'main',
  'main.cpp',
  link_with: [interaction_db_lib, prompt_processor, llm_interface, api_manager_lib],
  dependencies: [curl_dep, json_dep, sqlite3_dep, openssl_dep],
)
