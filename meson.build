project('main_loop', 'cpp')

subdir('interaction_loop')
subdir('prompt_processor')
subdir('llm_interface')

executable('main', 'main.cpp', link_with: [interaction_loop, prompt_processor, llm_interface])

