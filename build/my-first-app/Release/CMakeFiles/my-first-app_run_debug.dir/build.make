# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.15.5/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.15.5/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/sambourgault/Documents/UCSB/WINTER2020/MAT240B_2020/allolib_playground

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/sambourgault/Documents/UCSB/WINTER2020/MAT240B_2020/allolib_playground/sambourgault/build/my-first-app/Release

# Utility rule file for my-first-app_run_debug.

# Include the progress variables for this target.
include CMakeFiles/my-first-app_run_debug.dir/progress.make

CMakeFiles/my-first-app_run_debug: ../../../bin/my-first-app
	cd /Users/sambourgault/Documents/UCSB/WINTER2020/MAT240B_2020/allolib_playground/sambourgault/bin && lldb -o\ run ./my-first-appd

my-first-app_run_debug: CMakeFiles/my-first-app_run_debug
my-first-app_run_debug: CMakeFiles/my-first-app_run_debug.dir/build.make

.PHONY : my-first-app_run_debug

# Rule to build all files generated by this target.
CMakeFiles/my-first-app_run_debug.dir/build: my-first-app_run_debug

.PHONY : CMakeFiles/my-first-app_run_debug.dir/build

CMakeFiles/my-first-app_run_debug.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/my-first-app_run_debug.dir/cmake_clean.cmake
.PHONY : CMakeFiles/my-first-app_run_debug.dir/clean

CMakeFiles/my-first-app_run_debug.dir/depend:
	cd /Users/sambourgault/Documents/UCSB/WINTER2020/MAT240B_2020/allolib_playground/sambourgault/build/my-first-app/Release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/sambourgault/Documents/UCSB/WINTER2020/MAT240B_2020/allolib_playground /Users/sambourgault/Documents/UCSB/WINTER2020/MAT240B_2020/allolib_playground /Users/sambourgault/Documents/UCSB/WINTER2020/MAT240B_2020/allolib_playground/sambourgault/build/my-first-app/Release /Users/sambourgault/Documents/UCSB/WINTER2020/MAT240B_2020/allolib_playground/sambourgault/build/my-first-app/Release /Users/sambourgault/Documents/UCSB/WINTER2020/MAT240B_2020/allolib_playground/sambourgault/build/my-first-app/Release/CMakeFiles/my-first-app_run_debug.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/my-first-app_run_debug.dir/depend

