# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hjkim/PiGraph/bin

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hjkim/PiGraph/bin

# Include any dependencies generated for this target.
include CMakeFiles/wm.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/wm.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/wm.dir/flags.make

CMakeFiles/wm.dir/wm_main.cpp.o: CMakeFiles/wm.dir/flags.make
CMakeFiles/wm.dir/wm_main.cpp.o: wm_main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hjkim/PiGraph/bin/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/wm.dir/wm_main.cpp.o"
	/usr/local/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wm.dir/wm_main.cpp.o -c /home/hjkim/PiGraph/bin/wm_main.cpp

CMakeFiles/wm.dir/wm_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wm.dir/wm_main.cpp.i"
	/usr/local/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hjkim/PiGraph/bin/wm_main.cpp > CMakeFiles/wm.dir/wm_main.cpp.i

CMakeFiles/wm.dir/wm_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wm.dir/wm_main.cpp.s"
	/usr/local/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hjkim/PiGraph/bin/wm_main.cpp -o CMakeFiles/wm.dir/wm_main.cpp.s

# Object files for target wm
wm_OBJECTS = \
"CMakeFiles/wm.dir/wm_main.cpp.o"

# External object files for target wm
wm_EXTERNAL_OBJECTS =

wm: CMakeFiles/wm.dir/wm_main.cpp.o
wm: CMakeFiles/wm.dir/build.make
wm: module/libResource.a
wm: module/libExecution.a
wm: module/lib/libzk.a
wm: CMakeFiles/wm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hjkim/PiGraph/bin/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable wm"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/wm.dir/build: wm

.PHONY : CMakeFiles/wm.dir/build

CMakeFiles/wm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/wm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/wm.dir/clean

CMakeFiles/wm.dir/depend:
	cd /home/hjkim/PiGraph/bin && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hjkim/PiGraph/bin /home/hjkim/PiGraph/bin /home/hjkim/PiGraph/bin /home/hjkim/PiGraph/bin /home/hjkim/PiGraph/bin/CMakeFiles/wm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/wm.dir/depend
