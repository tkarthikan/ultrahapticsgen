cmake_minimum_required(VERSION 3.3)

project(UltraleapHapticsExamples)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
	message(FATAL_ERROR "Run CMake in a separate build directory to avoid source code contamination.")
endif ()

# For FindLeap.cmake
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

find_package(UltraleapHaptics REQUIRED)

option(LEAPC_EXAMPLES "Build LeapC Examples (Windows only)" OFF)
option(CONVERTER_EXAMPLES "Build Examples using Leap Hand Converter" ON)

if (LEAPC_EXAMPLES)
	if(WIN32)
		find_package(LeapSDK PATHS "${LEAP_ROOT}")
		if (LeapSDK_FOUND)
			get_target_property(LEAPC_DLL LeapSDK::LeapC IMPORTED_LOCATION_RELEASE)
			get_target_property(LEAPC_LIB LeapSDK::LeapC IMPORTED_IMPLIB_RELEASE)
		else()
			message(WARNING "Could not find LeapC SDK")
		endif()
	else()
		message(WARNING "Unable to build LeapC examples, currently Windows only.")
	endif()
else()
	find_package(Leap)
	if (LEAP_FOUND)
		include_directories(${LEAP_INCLUDE_DIR})
	else ()
		message(WARNING "Could not find Leap SDK")
	endif ()
endif()

add_subdirectory(cpp)

message(STATUS "	========== Examples Configuration Summary =========\n")

if (LEAPC_EXAMPLES)
    message(STATUS "	Leap:             Not Enabled")
    if (LeapSDK_FOUND)
        message(STATUS "	LeapC:            Enabled")
    else ()
        message(STATUS "	LeapC:            Not Found")
    endif ()
else()
    if (LEAP_FOUND)
        message(STATUS "	Leap:             Enabled")
    else ()
        message(STATUS "	Leap:             Not Found")
    endif ()
    message(STATUS "	LeapC:            Not Enabled")
endif()

message(STATUS "")
