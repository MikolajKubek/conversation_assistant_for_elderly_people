curl_dep = dependency('libcurl')
json_dep = dependency('nlohmann_json')
llm_interface = library('llm_interface', 'llm_interface.cpp', install: true, dependencies: [curl_dep, json_dep])
