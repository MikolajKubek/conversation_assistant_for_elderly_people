project('main_loop', 'cpp')

subdir('llm_interface')
subdir('interaction_loop')
subdir('prompt_processor')

curl_dep = dependency('libcurl')
inja_proj = subproject('inja')
inja_dep = inja_proj.get_variable('inja_dep')

executable('main', 'main.cpp', 
  link_with: [
    interaction_loop,
    prompt_processor,
    llm_interface
    ],
  dependencies: [
    curl_dep,
    inja_dep
    ]
  )

