# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mikolaj/cpp/altenpfleger-loop

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mikolaj/cpp/altenpfleger-loop/build

# Include any dependencies generated for this target.
include prompt_processor/CMakeFiles/PromptProcessor.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include prompt_processor/CMakeFiles/PromptProcessor.dir/compiler_depend.make

# Include the progress variables for this target.
include prompt_processor/CMakeFiles/PromptProcessor.dir/progress.make

# Include the compile flags for this target's objects.
include prompt_processor/CMakeFiles/PromptProcessor.dir/flags.make

prompt_processor/CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.o: prompt_processor/CMakeFiles/PromptProcessor.dir/flags.make
prompt_processor/CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.o: /home/mikolaj/cpp/altenpfleger-loop/prompt_processor/prompt_processor.cpp
prompt_processor/CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.o: prompt_processor/CMakeFiles/PromptProcessor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/mikolaj/cpp/altenpfleger-loop/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object prompt_processor/CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.o"
	cd /home/mikolaj/cpp/altenpfleger-loop/build/prompt_processor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT prompt_processor/CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.o -MF CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.o.d -o CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.o -c /home/mikolaj/cpp/altenpfleger-loop/prompt_processor/prompt_processor.cpp

prompt_processor/CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.i"
	cd /home/mikolaj/cpp/altenpfleger-loop/build/prompt_processor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mikolaj/cpp/altenpfleger-loop/prompt_processor/prompt_processor.cpp > CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.i

prompt_processor/CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.s"
	cd /home/mikolaj/cpp/altenpfleger-loop/build/prompt_processor && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mikolaj/cpp/altenpfleger-loop/prompt_processor/prompt_processor.cpp -o CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.s

# Object files for target PromptProcessor
PromptProcessor_OBJECTS = \
"CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.o"

# External object files for target PromptProcessor
PromptProcessor_EXTERNAL_OBJECTS =

prompt_processor/libPromptProcessor.a: prompt_processor/CMakeFiles/PromptProcessor.dir/prompt_processor.cpp.o
prompt_processor/libPromptProcessor.a: prompt_processor/CMakeFiles/PromptProcessor.dir/build.make
prompt_processor/libPromptProcessor.a: prompt_processor/CMakeFiles/PromptProcessor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/mikolaj/cpp/altenpfleger-loop/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libPromptProcessor.a"
	cd /home/mikolaj/cpp/altenpfleger-loop/build/prompt_processor && $(CMAKE_COMMAND) -P CMakeFiles/PromptProcessor.dir/cmake_clean_target.cmake
	cd /home/mikolaj/cpp/altenpfleger-loop/build/prompt_processor && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PromptProcessor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
prompt_processor/CMakeFiles/PromptProcessor.dir/build: prompt_processor/libPromptProcessor.a
.PHONY : prompt_processor/CMakeFiles/PromptProcessor.dir/build

prompt_processor/CMakeFiles/PromptProcessor.dir/clean:
	cd /home/mikolaj/cpp/altenpfleger-loop/build/prompt_processor && $(CMAKE_COMMAND) -P CMakeFiles/PromptProcessor.dir/cmake_clean.cmake
.PHONY : prompt_processor/CMakeFiles/PromptProcessor.dir/clean

prompt_processor/CMakeFiles/PromptProcessor.dir/depend:
	cd /home/mikolaj/cpp/altenpfleger-loop/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mikolaj/cpp/altenpfleger-loop /home/mikolaj/cpp/altenpfleger-loop/prompt_processor /home/mikolaj/cpp/altenpfleger-loop/build /home/mikolaj/cpp/altenpfleger-loop/build/prompt_processor /home/mikolaj/cpp/altenpfleger-loop/build/prompt_processor/CMakeFiles/PromptProcessor.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : prompt_processor/CMakeFiles/PromptProcessor.dir/depend

