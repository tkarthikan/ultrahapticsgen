cmake_minimum_required(VERSION 3.3)

project(TestProject)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
	message(FATAL_ERROR "Run CMake in a separate build directory to avoid source code contamination.")
endif ()

# For FindLeap.cmake
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

find_package(UltraleapHaptics REQUIRED)

# Find the leap package using `FindLeap.cmake`.
set(CMAKE_CXX_STANDARD 17)

if (WIN32)
	# Windows requires the runtime library to be found in the current directory
	add_custom_target(copy_ulhaptics_libs
		COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULHAPTICS_RUNTIME_LIBRARY}" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULHAPTICS_LIBUSB_DLL}" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
	)
endif()

if (WIN32)
	add_library(uhdriver SHARED main.cpp)
	target_link_libraries(uhdriver Ultraleap::Haptics)
	add_dependencies(uhdriver copy_ulhaptics_libs)
endif()