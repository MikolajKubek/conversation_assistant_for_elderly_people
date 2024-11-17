project('main_loop', 'cpp')

subdir('api_manager')
subdir('llm_interface')
subdir('prompt_processor')

curl_dep = dependency('libcurl')

executable('main', 'main.cpp',
  link_with: [
    prompt_processor,
    llm_interface,
    api_manager
    ],
  dependencies: [
    curl_dep,
    ]
  )
