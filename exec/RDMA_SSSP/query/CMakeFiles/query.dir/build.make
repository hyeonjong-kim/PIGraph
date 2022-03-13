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
CMAKE_SOURCE_DIR = /home/hjkim/PiGraph/rdma/SSSP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hjkim/PiGraph/exec/RDMA_SSSP

# Include any dependencies generated for this target.
include query/CMakeFiles/query.dir/depend.make

# Include the progress variables for this target.
include query/CMakeFiles/query.dir/progress.make

# Include the compile flags for this target's objects.
include query/CMakeFiles/query.dir/flags.make

query/CMakeFiles/query.dir/PageRank.cpp.o: query/CMakeFiles/query.dir/flags.make
query/CMakeFiles/query.dir/PageRank.cpp.o: /home/hjkim/PiGraph/rdma/src/query/PageRank.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hjkim/PiGraph/exec/RDMA_SSSP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object query/CMakeFiles/query.dir/PageRank.cpp.o"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/query.dir/PageRank.cpp.o -c /home/hjkim/PiGraph/rdma/src/query/PageRank.cpp

query/CMakeFiles/query.dir/PageRank.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/query.dir/PageRank.cpp.i"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hjkim/PiGraph/rdma/src/query/PageRank.cpp > CMakeFiles/query.dir/PageRank.cpp.i

query/CMakeFiles/query.dir/PageRank.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/query.dir/PageRank.cpp.s"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hjkim/PiGraph/rdma/src/query/PageRank.cpp -o CMakeFiles/query.dir/PageRank.cpp.s

query/CMakeFiles/query.dir/SingleSourceShortestPath.cpp.o: query/CMakeFiles/query.dir/flags.make
query/CMakeFiles/query.dir/SingleSourceShortestPath.cpp.o: /home/hjkim/PiGraph/rdma/src/query/SingleSourceShortestPath.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hjkim/PiGraph/exec/RDMA_SSSP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object query/CMakeFiles/query.dir/SingleSourceShortestPath.cpp.o"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/query.dir/SingleSourceShortestPath.cpp.o -c /home/hjkim/PiGraph/rdma/src/query/SingleSourceShortestPath.cpp

query/CMakeFiles/query.dir/SingleSourceShortestPath.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/query.dir/SingleSourceShortestPath.cpp.i"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hjkim/PiGraph/rdma/src/query/SingleSourceShortestPath.cpp > CMakeFiles/query.dir/SingleSourceShortestPath.cpp.i

query/CMakeFiles/query.dir/SingleSourceShortestPath.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/query.dir/SingleSourceShortestPath.cpp.s"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hjkim/PiGraph/rdma/src/query/SingleSourceShortestPath.cpp -o CMakeFiles/query.dir/SingleSourceShortestPath.cpp.s

query/CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.o: query/CMakeFiles/query.dir/flags.make
query/CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.o: /home/hjkim/PiGraph/rdma/src/query/WeaklyConnectedComponent.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hjkim/PiGraph/exec/RDMA_SSSP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object query/CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.o"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.o -c /home/hjkim/PiGraph/rdma/src/query/WeaklyConnectedComponent.cpp

query/CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.i"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hjkim/PiGraph/rdma/src/query/WeaklyConnectedComponent.cpp > CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.i

query/CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.s"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hjkim/PiGraph/rdma/src/query/WeaklyConnectedComponent.cpp -o CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.s

# Object files for target query
query_OBJECTS = \
"CMakeFiles/query.dir/PageRank.cpp.o" \
"CMakeFiles/query.dir/SingleSourceShortestPath.cpp.o" \
"CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.o"

# External object files for target query
query_EXTERNAL_OBJECTS =

query/libquery.a: query/CMakeFiles/query.dir/PageRank.cpp.o
query/libquery.a: query/CMakeFiles/query.dir/SingleSourceShortestPath.cpp.o
query/libquery.a: query/CMakeFiles/query.dir/WeaklyConnectedComponent.cpp.o
query/libquery.a: query/CMakeFiles/query.dir/build.make
query/libquery.a: query/CMakeFiles/query.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hjkim/PiGraph/exec/RDMA_SSSP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libquery.a"
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && $(CMAKE_COMMAND) -P CMakeFiles/query.dir/cmake_clean_target.cmake
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/query.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
query/CMakeFiles/query.dir/build: query/libquery.a

.PHONY : query/CMakeFiles/query.dir/build

query/CMakeFiles/query.dir/clean:
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP/query && $(CMAKE_COMMAND) -P CMakeFiles/query.dir/cmake_clean.cmake
.PHONY : query/CMakeFiles/query.dir/clean

query/CMakeFiles/query.dir/depend:
	cd /home/hjkim/PiGraph/exec/RDMA_SSSP && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hjkim/PiGraph/rdma/SSSP /home/hjkim/PiGraph/rdma/src/query /home/hjkim/PiGraph/exec/RDMA_SSSP /home/hjkim/PiGraph/exec/RDMA_SSSP/query /home/hjkim/PiGraph/exec/RDMA_SSSP/query/CMakeFiles/query.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : query/CMakeFiles/query.dir/depend

