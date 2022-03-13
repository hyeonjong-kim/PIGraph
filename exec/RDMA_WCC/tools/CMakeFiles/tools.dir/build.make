# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hjkim/PiGraph/rdma/WCC

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hjkim/PiGraph/exec/RDMA_WCC

# Include any dependencies generated for this target.
include tools/CMakeFiles/tools.dir/depend.make

# Include the progress variables for this target.
include tools/CMakeFiles/tools.dir/progress.make

# Include the compile flags for this target's objects.
include tools/CMakeFiles/tools.dir/flags.make

tools/CMakeFiles/tools.dir/Parser.cpp.o: tools/CMakeFiles/tools.dir/flags.make
tools/CMakeFiles/tools.dir/Parser.cpp.o: /home/hjkim/PiGraph/rdma/src/tools/Parser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hjkim/PiGraph/exec/RDMA_WCC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tools/CMakeFiles/tools.dir/Parser.cpp.o"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tools.dir/Parser.cpp.o -c /home/hjkim/PiGraph/rdma/src/tools/Parser.cpp

tools/CMakeFiles/tools.dir/Parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tools.dir/Parser.cpp.i"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hjkim/PiGraph/rdma/src/tools/Parser.cpp > CMakeFiles/tools.dir/Parser.cpp.i

tools/CMakeFiles/tools.dir/Parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tools.dir/Parser.cpp.s"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hjkim/PiGraph/rdma/src/tools/Parser.cpp -o CMakeFiles/tools.dir/Parser.cpp.s

tools/CMakeFiles/tools.dir/ResourceChecker.cpp.o: tools/CMakeFiles/tools.dir/flags.make
tools/CMakeFiles/tools.dir/ResourceChecker.cpp.o: /home/hjkim/PiGraph/rdma/src/tools/ResourceChecker.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hjkim/PiGraph/exec/RDMA_WCC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object tools/CMakeFiles/tools.dir/ResourceChecker.cpp.o"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tools.dir/ResourceChecker.cpp.o -c /home/hjkim/PiGraph/rdma/src/tools/ResourceChecker.cpp

tools/CMakeFiles/tools.dir/ResourceChecker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tools.dir/ResourceChecker.cpp.i"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hjkim/PiGraph/rdma/src/tools/ResourceChecker.cpp > CMakeFiles/tools.dir/ResourceChecker.cpp.i

tools/CMakeFiles/tools.dir/ResourceChecker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tools.dir/ResourceChecker.cpp.s"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hjkim/PiGraph/rdma/src/tools/ResourceChecker.cpp -o CMakeFiles/tools.dir/ResourceChecker.cpp.s

tools/CMakeFiles/tools.dir/ThreadPool.cpp.o: tools/CMakeFiles/tools.dir/flags.make
tools/CMakeFiles/tools.dir/ThreadPool.cpp.o: /home/hjkim/PiGraph/rdma/src/tools/ThreadPool.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hjkim/PiGraph/exec/RDMA_WCC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object tools/CMakeFiles/tools.dir/ThreadPool.cpp.o"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tools.dir/ThreadPool.cpp.o -c /home/hjkim/PiGraph/rdma/src/tools/ThreadPool.cpp

tools/CMakeFiles/tools.dir/ThreadPool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tools.dir/ThreadPool.cpp.i"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hjkim/PiGraph/rdma/src/tools/ThreadPool.cpp > CMakeFiles/tools.dir/ThreadPool.cpp.i

tools/CMakeFiles/tools.dir/ThreadPool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tools.dir/ThreadPool.cpp.s"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hjkim/PiGraph/rdma/src/tools/ThreadPool.cpp -o CMakeFiles/tools.dir/ThreadPool.cpp.s

tools/CMakeFiles/tools.dir/argparse.cpp.o: tools/CMakeFiles/tools.dir/flags.make
tools/CMakeFiles/tools.dir/argparse.cpp.o: /home/hjkim/PiGraph/rdma/src/tools/argparse.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hjkim/PiGraph/exec/RDMA_WCC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object tools/CMakeFiles/tools.dir/argparse.cpp.o"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tools.dir/argparse.cpp.o -c /home/hjkim/PiGraph/rdma/src/tools/argparse.cpp

tools/CMakeFiles/tools.dir/argparse.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tools.dir/argparse.cpp.i"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hjkim/PiGraph/rdma/src/tools/argparse.cpp > CMakeFiles/tools.dir/argparse.cpp.i

tools/CMakeFiles/tools.dir/argparse.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tools.dir/argparse.cpp.s"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hjkim/PiGraph/rdma/src/tools/argparse.cpp -o CMakeFiles/tools.dir/argparse.cpp.s

# Object files for target tools
tools_OBJECTS = \
"CMakeFiles/tools.dir/Parser.cpp.o" \
"CMakeFiles/tools.dir/ResourceChecker.cpp.o" \
"CMakeFiles/tools.dir/ThreadPool.cpp.o" \
"CMakeFiles/tools.dir/argparse.cpp.o"

# External object files for target tools
tools_EXTERNAL_OBJECTS =

tools/libtools.a: tools/CMakeFiles/tools.dir/Parser.cpp.o
tools/libtools.a: tools/CMakeFiles/tools.dir/ResourceChecker.cpp.o
tools/libtools.a: tools/CMakeFiles/tools.dir/ThreadPool.cpp.o
tools/libtools.a: tools/CMakeFiles/tools.dir/argparse.cpp.o
tools/libtools.a: tools/CMakeFiles/tools.dir/build.make
tools/libtools.a: tools/CMakeFiles/tools.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hjkim/PiGraph/exec/RDMA_WCC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX static library libtools.a"
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && $(CMAKE_COMMAND) -P CMakeFiles/tools.dir/cmake_clean_target.cmake
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tools.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/CMakeFiles/tools.dir/build: tools/libtools.a

.PHONY : tools/CMakeFiles/tools.dir/build

tools/CMakeFiles/tools.dir/clean:
	cd /home/hjkim/PiGraph/exec/RDMA_WCC/tools && $(CMAKE_COMMAND) -P CMakeFiles/tools.dir/cmake_clean.cmake
.PHONY : tools/CMakeFiles/tools.dir/clean

tools/CMakeFiles/tools.dir/depend:
	cd /home/hjkim/PiGraph/exec/RDMA_WCC && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hjkim/PiGraph/rdma/WCC /home/hjkim/PiGraph/rdma/src/tools /home/hjkim/PiGraph/exec/RDMA_WCC /home/hjkim/PiGraph/exec/RDMA_WCC/tools /home/hjkim/PiGraph/exec/RDMA_WCC/tools/CMakeFiles/tools.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/CMakeFiles/tools.dir/depend

