# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.26.3/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.26.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Applications/CS488Project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Applications/CS488Project/build

# Include any dependencies generated for this target.
include CMakeFiles/v-cloud.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/v-cloud.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/v-cloud.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/v-cloud.dir/flags.make

CMakeFiles/v-cloud.dir/src/libtarga.c.o: CMakeFiles/v-cloud.dir/flags.make
CMakeFiles/v-cloud.dir/src/libtarga.c.o: /Applications/CS488Project/src/libtarga.c
CMakeFiles/v-cloud.dir/src/libtarga.c.o: CMakeFiles/v-cloud.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Applications/CS488Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/v-cloud.dir/src/libtarga.c.o"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/v-cloud.dir/src/libtarga.c.o -MF CMakeFiles/v-cloud.dir/src/libtarga.c.o.d -o CMakeFiles/v-cloud.dir/src/libtarga.c.o -c /Applications/CS488Project/src/libtarga.c

CMakeFiles/v-cloud.dir/src/libtarga.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/v-cloud.dir/src/libtarga.c.i"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Applications/CS488Project/src/libtarga.c > CMakeFiles/v-cloud.dir/src/libtarga.c.i

CMakeFiles/v-cloud.dir/src/libtarga.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/v-cloud.dir/src/libtarga.c.s"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Applications/CS488Project/src/libtarga.c -o CMakeFiles/v-cloud.dir/src/libtarga.c.s

CMakeFiles/v-cloud.dir/src/main.cpp.o: CMakeFiles/v-cloud.dir/flags.make
CMakeFiles/v-cloud.dir/src/main.cpp.o: /Applications/CS488Project/src/main.cpp
CMakeFiles/v-cloud.dir/src/main.cpp.o: CMakeFiles/v-cloud.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Applications/CS488Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/v-cloud.dir/src/main.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/v-cloud.dir/src/main.cpp.o -MF CMakeFiles/v-cloud.dir/src/main.cpp.o.d -o CMakeFiles/v-cloud.dir/src/main.cpp.o -c /Applications/CS488Project/src/main.cpp

CMakeFiles/v-cloud.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/v-cloud.dir/src/main.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Applications/CS488Project/src/main.cpp > CMakeFiles/v-cloud.dir/src/main.cpp.i

CMakeFiles/v-cloud.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/v-cloud.dir/src/main.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Applications/CS488Project/src/main.cpp -o CMakeFiles/v-cloud.dir/src/main.cpp.s

# Object files for target v-cloud
v__cloud_OBJECTS = \
"CMakeFiles/v-cloud.dir/src/libtarga.c.o" \
"CMakeFiles/v-cloud.dir/src/main.cpp.o"

# External object files for target v-cloud
v__cloud_EXTERNAL_OBJECTS =

v-cloud: CMakeFiles/v-cloud.dir/src/libtarga.c.o
v-cloud: CMakeFiles/v-cloud.dir/src/main.cpp.o
v-cloud: CMakeFiles/v-cloud.dir/build.make
v-cloud: external/glfw/src/libglfw3.a
v-cloud: external/glew/lib/libglew.a
v-cloud: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.1.sdk/System/Library/Frameworks/OpenGL.framework/OpenGL.tbd
v-cloud: CMakeFiles/v-cloud.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Applications/CS488Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable v-cloud"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/v-cloud.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/v-cloud.dir/build: v-cloud
.PHONY : CMakeFiles/v-cloud.dir/build

CMakeFiles/v-cloud.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/v-cloud.dir/cmake_clean.cmake
.PHONY : CMakeFiles/v-cloud.dir/clean

CMakeFiles/v-cloud.dir/depend:
	cd /Applications/CS488Project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Applications/CS488Project /Applications/CS488Project /Applications/CS488Project/build /Applications/CS488Project/build /Applications/CS488Project/build/CMakeFiles/v-cloud.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/v-cloud.dir/depend

