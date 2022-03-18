# Specify the minimum CMake version required to build this project
cmake_minimum_required(VERSION 3.14)

# IF(APPLE)
#   SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
# ENDIF(APPLE)

# Define the name of this project
project("TestProject")

set(CMAKE_OSX_ARCHITECTURES "x86_64")  #if I comment this, I get the error.
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

# # Defining minimum C++ version to C++11
# set_property(TARGET TestProject PROPERTY CXX_STANDARD_REQUIRED TRUE)
# set_property(TARGET TestProject PROPERTY CXX_STANDARD 11)
# set_property(TARGET TestProject PROPERTY CXX_EXTENSIONS OFF)

# Find the SDK and make it available
find_package(UltraleapHaptics REQUIRED)

#This is for running Testproject as executable
# Define the name of the executable, and specify that it is comprised of one file: main.cpp
#add_executable(TestProject "main.cpp")

add_library(uhdriver MODULE main.cpp)
target_link_libraries(uhdriver Ultraleap::Haptics)

# Link the SDK to the TestProject. This also sets up include directories
#target_link_libraries(TestProject Ultraleap::Haptics)

# On windows we will copy over the DLLs required to run applications using the SDK
if (WIN32)
    add_custom_target(copy_ulhaptics_libs
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULHAPTICS_RUNTIME_LIBRARY}" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULHAPTICS_LIBUSB_DLL}" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
    )

    add_dependencies(TestProject copy_ulhaptics_libs)
endif()