# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.2.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.2.2\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Katherine\doubles

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Katherine\doubles\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles\doubles.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\doubles.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\doubles.dir\flags.make

CMakeFiles\doubles.dir\main.c.obj: CMakeFiles\doubles.dir\flags.make
CMakeFiles\doubles.dir\main.c.obj: ..\main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Katherine\doubles\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/doubles.dir/main.c.obj"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1427~1.291\bin\Hostx64\x64\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\doubles.dir\main.c.obj /FdCMakeFiles\doubles.dir\ /FS -c C:\Users\Katherine\doubles\main.c
<<

CMakeFiles\doubles.dir\main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/doubles.dir/main.c.i"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1427~1.291\bin\Hostx64\x64\cl.exe > CMakeFiles\doubles.dir\main.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\Katherine\doubles\main.c
<<

CMakeFiles\doubles.dir\main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/doubles.dir/main.c.s"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1427~1.291\bin\Hostx64\x64\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\doubles.dir\main.c.s /c C:\Users\Katherine\doubles\main.c
<<

# Object files for target doubles
doubles_OBJECTS = \
"CMakeFiles\doubles.dir\main.c.obj"

# External object files for target doubles
doubles_EXTERNAL_OBJECTS =

doubles.exe: CMakeFiles\doubles.dir\main.c.obj
doubles.exe: CMakeFiles\doubles.dir\build.make
doubles.exe: CMakeFiles\doubles.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Katherine\doubles\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable doubles.exe"
	"C:\Program Files\JetBrains\CLion 2020.2.2\bin\cmake\win\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\doubles.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x64\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x64\mt.exe --manifests  -- C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1427~1.291\bin\Hostx64\x64\link.exe /nologo @CMakeFiles\doubles.dir\objects1.rsp @<<
 /out:doubles.exe /implib:doubles.lib /pdb:C:\Users\Katherine\doubles\cmake-build-debug\doubles.pdb /version:0.0  /machine:x64 /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<

# Rule to build all files generated by this target.
CMakeFiles\doubles.dir\build: doubles.exe

.PHONY : CMakeFiles\doubles.dir\build

CMakeFiles\doubles.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\doubles.dir\cmake_clean.cmake
.PHONY : CMakeFiles\doubles.dir\clean

CMakeFiles\doubles.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" C:\Users\Katherine\doubles C:\Users\Katherine\doubles C:\Users\Katherine\doubles\cmake-build-debug C:\Users\Katherine\doubles\cmake-build-debug C:\Users\Katherine\doubles\cmake-build-debug\CMakeFiles\doubles.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\doubles.dir\depend

