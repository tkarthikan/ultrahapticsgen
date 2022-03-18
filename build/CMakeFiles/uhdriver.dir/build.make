# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.22.2/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.22.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/karthik/Documents/GenHap/UH_Multithread

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/karthik/Documents/GenHap/UH_Multithread/build

# Include any dependencies generated for this target.
include CMakeFiles/uhdriver.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/uhdriver.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/uhdriver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/uhdriver.dir/flags.make

CMakeFiles/uhdriver.dir/main.cpp.o: CMakeFiles/uhdriver.dir/flags.make
CMakeFiles/uhdriver.dir/main.cpp.o: ../main.cpp
CMakeFiles/uhdriver.dir/main.cpp.o: CMakeFiles/uhdriver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/karthik/Documents/GenHap/UH_Multithread/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/uhdriver.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/uhdriver.dir/main.cpp.o -MF CMakeFiles/uhdriver.dir/main.cpp.o.d -o CMakeFiles/uhdriver.dir/main.cpp.o -c /Users/karthik/Documents/GenHap/UH_Multithread/main.cpp

CMakeFiles/uhdriver.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/uhdriver.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/karthik/Documents/GenHap/UH_Multithread/main.cpp > CMakeFiles/uhdriver.dir/main.cpp.i

CMakeFiles/uhdriver.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/uhdriver.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/karthik/Documents/GenHap/UH_Multithread/main.cpp -o CMakeFiles/uhdriver.dir/main.cpp.s

# Object files for target uhdriver
uhdriver_OBJECTS = \
"CMakeFiles/uhdriver.dir/main.cpp.o"

# External object files for target uhdriver
uhdriver_EXTERNAL_OBJECTS =

libuhdriver.so: CMakeFiles/uhdriver.dir/main.cpp.o
libuhdriver.so: CMakeFiles/uhdriver.dir/build.make
libuhdriver.so: /Library/Frameworks/UltraleapHaptics.framework/Versions/3.0/UltraleapHaptics
libuhdriver.so: CMakeFiles/uhdriver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/karthik/Documents/GenHap/UH_Multithread/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module libuhdriver.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/uhdriver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/uhdriver.dir/build: libuhdriver.so
.PHONY : CMakeFiles/uhdriver.dir/build

CMakeFiles/uhdriver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/uhdriver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/uhdriver.dir/clean

CMakeFiles/uhdriver.dir/depend:
	cd /Users/karthik/Documents/GenHap/UH_Multithread/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/karthik/Documents/GenHap/UH_Multithread /Users/karthik/Documents/GenHap/UH_Multithread /Users/karthik/Documents/GenHap/UH_Multithread/build /Users/karthik/Documents/GenHap/UH_Multithread/build /Users/karthik/Documents/GenHap/UH_Multithread/build/CMakeFiles/uhdriver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/uhdriver.dir/depend

