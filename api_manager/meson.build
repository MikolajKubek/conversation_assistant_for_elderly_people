curl_dep = dependency('libcurl')
json_dep = dependency('nlohmann_json')

api_manager = library('api_manager', 'api_manager.cpp', dependencies: [curl_dep, json_dep, interaction_db_dep], install: true)
