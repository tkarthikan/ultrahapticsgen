# Find the leap package using `FindLeap.cmake`.
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

# These examples do not require a Leap Motion.
set(basic_examples
	"Streaming_Circle"
	"Streaming_Line"
)

# These examples require the Leap SDK.
set(leap_examples

	)

# These examples require the leapC v4 SDK
set(leapc_examples
	
)

set(leap_converter_examples
	
)

if (WIN32)
	# Windows requires the runtime library to be found in the current directory
	add_custom_target(copy_ulhaptics_libs
		COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULHAPTICS_RUNTIME_LIBRARY}" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ULHAPTICS_LIBUSB_DLL}" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
	)
endif()

# Build the basic examples.
foreach (example IN LISTS basic_examples)
	add_executable("${example}" "${example}.cpp")
	target_link_libraries("${example}" Ultraleap::Haptics)
	if (WIN32)
		add_dependencies(${example} copy_ulhaptics_libs)
	endif()
endforeach ()

if (SENSATION_PACKAGE_DIR AND SENSATION_PACKAGE_PATH AND SENSATION_PACKAGE_NAME)
	message(STATUS "Sensation Package will be copied")
	set(COPY_SENSATION_PACKAGE TRUE)
else()
	message(STATUS "Sensation Package will not be copied")
	set(COPY_SENSATION_PACKAGE FALSE)
endif()

if (CONVERTER_EXAMPLES)
	if (LEAP_CONVERTER_DIR)
		message(STATUS "Using LeapHandConverter from ${LEAP_CONVERTER_DIR}")
		set(USE_EXTERNAL_LEAP_CONVERTER TRUE)
	else()
		set(USE_EXTERNAL_LEAP_CONVERTER FALSE)
	endif()
endif()

if (COPY_SENSATION_PACKAGE)
	add_custom_target(copy_package ALL)

	message(STATUS "Copying files from ${SENSATION_PACKAGE_DIR}/${SENSATION_PACKAGE_NAME} to ${CMAKE_CURRENT_BINARY_DIR}")
	add_custom_command (
		TARGET copy_package POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${SENSATION_PACKAGE_DIR}/${SENSATION_PACKAGE_NAME}/"
		"${CMAKE_CURRENT_BINARY_DIR}/${SENSATION_PACKAGE_NAME}/")

	add_custom_command (
		TARGET copy_package POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy
		"${SENSATION_PACKAGE_PATH}/${SENSATION_PACKAGE_NAME}.ssp"
		"${CMAKE_CURRENT_BINARY_DIR}/${SENSATION_PACKAGE_NAME}.ssp")

	#this is needed as the working directory in the IDE in windows is CMAKE_CURRENT_BINARY_DIR, 
	#but when outputing examples it is CMAKE_CURRENT_BINARY_DIR/$<CONFIGURATION>
	IF (WIN32)
		set(debug_dir "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>")

		message(STATUS "Copying files from ${SENSATION_PACKAGE_DIR} to ${debug_dir}")
		add_custom_command (
			TARGET copy_package POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${SENSATION_PACKAGE_DIR}/${SENSATION_PACKAGE_NAME}/"
			"${debug_dir}/${SENSATION_PACKAGE_NAME}/")

		message(STATUS "Copying file from ${SENSATION_PACKAGE_PATH}/${SENSATION_PACKAGE_NAME}.ssp to ${debug_dir}")
		add_custom_command (
			TARGET copy_package POST_BUILD 
			COMMAND ${CMAKE_COMMAND} -E copy
			"${SENSATION_PACKAGE_PATH}/${SENSATION_PACKAGE_NAME}.ssp"
			"${debug_dir}/${SENSATION_PACKAGE_NAME}.ssp")
	ENDIF()
endif()

# Build the Leap Motion examples, if the Leap SDK was found.
if (LEAP_FOUND)
	# Copy the Leap Motion library into the examples folder for convenience.
	add_custom_target(copy_leap_libs
		COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/$<$<PLATFORM_ID:Windows>:$<CONFIG>>"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different $<TARGET_FILE:Leap::Leap> "${CMAKE_CURRENT_BINARY_DIR}/$<$<PLATFORM_ID:Windows>:$<CONFIG>>"
	)
	foreach (example IN LISTS leap_examples)
		add_executable("${example}" "${example}.cpp")
		target_link_libraries("${example}" Ultraleap::Haptics Leap::Leap)
		add_dependencies(${example} copy_leap_libs)
		if (WIN32)
			add_dependencies(${example} copy_ulhaptics_libs)
		endif()
		# When using libstdc++ we need to link with libatomic to enable the use
		# of large std::atomic's. libstdc++ is only used with GCC, or by default
		# with Clang on Linux.
		if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (UNIX AND NOT APPLE))
			target_link_libraries("${example}" atomic)
		endif ()
	endforeach()

	if (CONVERTER_EXAMPLES)
		foreach (example IN LISTS leap_converter_examples)
			add_executable("${example}" "${example}.cpp")
			if (USE_EXTERNAL_LEAP_CONVERTER)
				target_include_directories("${example}" PRIVATE "${LEAP_CONVERTER_DIR}")
			endif()
			target_link_libraries("${example}" Ultraleap::Haptics Leap::Leap)
			add_dependencies(${example} copy_leap_libs)
			if (WIN32)
				add_dependencies(${example} copy_ulhaptics_libs)
			endif()
			# When using libstdc++ we need to link with libatomic to enable the use
			# of large std::atomic's. libstdc++ is only used with GCC, or by default
			# with Clang on Linux.
			if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (UNIX AND NOT APPLE))
				target_link_libraries("${example}" atomic)
			endif ()
		endforeach()
	else()
		message("Did not get leap converter dir")
	endif()


endif()

if (LeapSDK_FOUND)

	add_custom_target(copy_leapc_libs
			COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/$<$<PLATFORM_ID:Windows>:$<CONFIG>>"
			COMMAND "${CMAKE_COMMAND}" -E copy_if_different $<TARGET_PROPERTY:LeapSDK::LeapC,IMPORTED_LOCATION_RELEASE> "${CMAKE_CURRENT_BINARY_DIR}/$<$<PLATFORM_ID:Windows>:$<CONFIG>>"
	)
	foreach(example IN LISTS leapc_examples)
		add_executable("${example}" "${example}.cpp" "LeapC/samples/ExampleConnection.c" "LeapC/samples/ExampleConnection.h")
		target_include_directories(${example} PUBLIC ${LEAP_ROOT}/include)
        target_include_directories(${example} PUBLIC "LeapC/samples")
		target_link_libraries("${example}" Ultraleap::Haptics LeapSDK::LeapC)
		add_dependencies(${example} copy_leapc_libs)
		if (WIN32)
			add_dependencies(${example} copy_ulhaptics_libs)
		endif()
		if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (UNIX AND NOT APPLE))
			target_link_libraries("${example}" atomic)
		endif ()
	endforeach()
endif()
