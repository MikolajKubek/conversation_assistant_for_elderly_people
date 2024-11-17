project('main_loop', 'cpp')

subdir('llm_interface')
subdir('prompt_processor')

curl_dep = dependency('libcurl')

executable('main', 'main.cpp',
  link_with: [
    prompt_processor,
    llm_interface
    ],
  dependencies: [
    curl_dep,
    ]
  )
