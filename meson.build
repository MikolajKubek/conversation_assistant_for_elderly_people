project('main_loop', 'cpp')

subdir('llm_interface')
subdir('interaction_loop')
subdir('prompt_processor')

curl_dep = dependency('libcurl')

executable('main', 'main.cpp',
  link_with: [
    interaction_loop,
    prompt_processor,
    llm_interface
    ],
  dependencies: [
    curl_dep,
    ]
  )
