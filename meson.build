project('main_loop', 'cpp')

subdir('api_manager')
subdir('llm_interface')
subdir('prompt_processor')

curl_dep = dependency('libcurl')
json_dep = dependency('nlohmann_json')
openssl_dep = dependency('openssl')
sqlite3_dep = dependency('sqlite3')

executable(
  'main',
  'main.cpp',
  link_with: [prompt_processor, llm_interface, api_manager],
  dependencies: [curl_dep, json_dep, sqlite3_dep, openssl_dep],
)
