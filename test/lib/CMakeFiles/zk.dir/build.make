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
CMAKE_SOURCE_DIR = /home/hjkim/PiGraph/test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hjkim/PiGraph/test

# Include any dependencies generated for this target.
include lib/CMakeFiles/zk.dir/depend.make

# Include the progress variables for this target.
include lib/CMakeFiles/zk.dir/progress.make

# Include the compile flags for this target's objects.
include lib/CMakeFiles/zk.dir/flags.make

lib/CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.o: lib/CMakeFiles/zk.dir/flags.make
lib/CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.o: /home/hjkim/PiGraph/src/zk/zkTools.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hjkim/PiGraph/test/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.o"
	cd /home/hjkim/PiGraph/test/lib && /usr/local/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.o -c /home/hjkim/PiGraph/src/zk/zkTools.cpp

lib/CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.i"
	cd /home/hjkim/PiGraph/test/lib && /usr/local/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hjkim/PiGraph/src/zk/zkTools.cpp > CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.i

lib/CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.s"
	cd /home/hjkim/PiGraph/test/lib && /usr/local/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hjkim/PiGraph/src/zk/zkTools.cpp -o CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.s

# Object files for target zk
zk_OBJECTS = \
"CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.o"

# External object files for target zk
zk_EXTERNAL_OBJECTS =

lib/libzk.a: lib/CMakeFiles/zk.dir/home/hjkim/PiGraph/src/zk/zkTools.cpp.o
lib/libzk.a: lib/CMakeFiles/zk.dir/build.make
lib/libzk.a: lib/CMakeFiles/zk.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hjkim/PiGraph/test/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libzk.a"
	cd /home/hjkim/PiGraph/test/lib && $(CMAKE_COMMAND) -P CMakeFiles/zk.dir/cmake_clean_target.cmake
	cd /home/hjkim/PiGraph/test/lib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/zk.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/CMakeFiles/zk.dir/build: lib/libzk.a

.PHONY : lib/CMakeFiles/zk.dir/build

lib/CMakeFiles/zk.dir/clean:
	cd /home/hjkim/PiGraph/test/lib && $(CMAKE_COMMAND) -P CMakeFiles/zk.dir/cmake_clean.cmake
.PHONY : lib/CMakeFiles/zk.dir/clean

lib/CMakeFiles/zk.dir/depend:
	cd /home/hjkim/PiGraph/test && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hjkim/PiGraph/test /home/hjkim/PiGraph/lib /home/hjkim/PiGraph/test /home/hjkim/PiGraph/test/lib /home/hjkim/PiGraph/test/lib/CMakeFiles/zk.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/CMakeFiles/zk.dir/depend
