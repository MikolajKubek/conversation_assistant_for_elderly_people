inja_proj = subproject('inja')
inja_dep = inja_proj.get_variable('inja_dep')
prompt_processor = library('prompt_processor', 'prompt_processor.cpp', install: true, dependencies: inja_dep)
