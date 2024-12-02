curl_dep = dependency('libcurl')
openssl_dep = dependency('openssl')

json_dep = dependency('nlohmann_json')
llm_interface = library('llm_interface', 'llm_interface.cpp', install: true, dependencies: [openssl_dep, curl_dep, json_dep])
