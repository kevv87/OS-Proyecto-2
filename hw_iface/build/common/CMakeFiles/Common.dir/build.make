# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.21.0/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.21.0/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build

# Include any dependencies generated for this target.
include common/CMakeFiles/Common.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include common/CMakeFiles/Common.dir/compiler_depend.make

# Include the progress variables for this target.
include common/CMakeFiles/Common.dir/progress.make

# Include the compile flags for this target's objects.
include common/CMakeFiles/Common.dir/flags.make

common/CMakeFiles/Common.dir/helloWorld.c.o: common/CMakeFiles/Common.dir/flags.make
common/CMakeFiles/Common.dir/helloWorld.c.o: /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/common/helloWorld.c
common/CMakeFiles/Common.dir/helloWorld.c.o: common/CMakeFiles/Common.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object common/CMakeFiles/Common.dir/helloWorld.c.o"
	cd /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/common && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT common/CMakeFiles/Common.dir/helloWorld.c.o -MF CMakeFiles/Common.dir/helloWorld.c.o.d -o CMakeFiles/Common.dir/helloWorld.c.o -c /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/common/helloWorld.c

common/CMakeFiles/Common.dir/helloWorld.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Common.dir/helloWorld.c.i"
	cd /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/common && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/common/helloWorld.c > CMakeFiles/Common.dir/helloWorld.c.i

common/CMakeFiles/Common.dir/helloWorld.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Common.dir/helloWorld.c.s"
	cd /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/common && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/common/helloWorld.c -o CMakeFiles/Common.dir/helloWorld.c.s

# Object files for target Common
Common_OBJECTS = \
"CMakeFiles/Common.dir/helloWorld.c.o"

# External object files for target Common
Common_EXTERNAL_OBJECTS =

common/libCommon.a: common/CMakeFiles/Common.dir/helloWorld.c.o
common/libCommon.a: common/CMakeFiles/Common.dir/build.make
common/libCommon.a: common/CMakeFiles/Common.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libCommon.a"
	cd /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/common && $(CMAKE_COMMAND) -P CMakeFiles/Common.dir/cmake_clean_target.cmake
	cd /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/common && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Common.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
common/CMakeFiles/Common.dir/build: common/libCommon.a
.PHONY : common/CMakeFiles/Common.dir/build

common/CMakeFiles/Common.dir/clean:
	cd /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/common && $(CMAKE_COMMAND) -P CMakeFiles/Common.dir/cmake_clean.cmake
.PHONY : common/CMakeFiles/Common.dir/clean

common/CMakeFiles/Common.dir/depend:
	cd /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/common /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/common /Users/kevv87/ghq/github.com/kevv87/OS-Proyecto-2/hw_iface/build/common/CMakeFiles/Common.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : common/CMakeFiles/Common.dir/depend
