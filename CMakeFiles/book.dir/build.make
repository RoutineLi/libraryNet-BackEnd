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


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/vrvuser/Book-System

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/vrvuser/Book-System

# Include any dependencies generated for this target.
include CMakeFiles/book.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/book.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/book.dir/flags.make

CMakeFiles/book.dir/module-src/book_module.cpp.o: CMakeFiles/book.dir/flags.make
CMakeFiles/book.dir/module-src/book_module.cpp.o: module-src/book_module.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/vrvuser/Book-System/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/book.dir/module-src/book_module.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/book.dir/module-src/book_module.cpp.o -c /home/vrvuser/Book-System/module-src/book_module.cpp

CMakeFiles/book.dir/module-src/book_module.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/book.dir/module-src/book_module.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/vrvuser/Book-System/module-src/book_module.cpp > CMakeFiles/book.dir/module-src/book_module.cpp.i

CMakeFiles/book.dir/module-src/book_module.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/book.dir/module-src/book_module.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/vrvuser/Book-System/module-src/book_module.cpp -o CMakeFiles/book.dir/module-src/book_module.cpp.s

CMakeFiles/book.dir/module-src/book_servlet.cpp.o: CMakeFiles/book.dir/flags.make
CMakeFiles/book.dir/module-src/book_servlet.cpp.o: module-src/book_servlet.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/vrvuser/Book-System/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/book.dir/module-src/book_servlet.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/book.dir/module-src/book_servlet.cpp.o -c /home/vrvuser/Book-System/module-src/book_servlet.cpp

CMakeFiles/book.dir/module-src/book_servlet.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/book.dir/module-src/book_servlet.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/vrvuser/Book-System/module-src/book_servlet.cpp > CMakeFiles/book.dir/module-src/book_servlet.cpp.i

CMakeFiles/book.dir/module-src/book_servlet.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/book.dir/module-src/book_servlet.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/vrvuser/Book-System/module-src/book_servlet.cpp -o CMakeFiles/book.dir/module-src/book_servlet.cpp.s

# Object files for target book
book_OBJECTS = \
"CMakeFiles/book.dir/module-src/book_module.cpp.o" \
"CMakeFiles/book.dir/module-src/book_servlet.cpp.o"

# External object files for target book
book_EXTERNAL_OBJECTS =

bin/module/libbook.so: CMakeFiles/book.dir/module-src/book_module.cpp.o
bin/module/libbook.so: CMakeFiles/book.dir/module-src/book_servlet.cpp.o
bin/module/libbook.so: CMakeFiles/book.dir/build.make
bin/module/libbook.so: CMakeFiles/book.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/vrvuser/Book-System/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library bin/module/libbook.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/book.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/book.dir/build: bin/module/libbook.so

.PHONY : CMakeFiles/book.dir/build

CMakeFiles/book.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/book.dir/cmake_clean.cmake
.PHONY : CMakeFiles/book.dir/clean

CMakeFiles/book.dir/depend:
	cd /home/vrvuser/Book-System && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/vrvuser/Book-System /home/vrvuser/Book-System /home/vrvuser/Book-System /home/vrvuser/Book-System /home/vrvuser/Book-System/CMakeFiles/book.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/book.dir/depend
