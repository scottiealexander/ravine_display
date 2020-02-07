# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (ravine_display)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(
    src/utilities/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
    ${GLEW_LIBRARIES}
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# grating script
add_executable(run_grating
    src/grating.cpp

    src/utilities/initialize.cpp
    src/utilities/initialize.hpp

    src/utilities/program.cpp
    src/utilities/program.hpp

    src/utilities/parse_ini.cpp
    src/utilities/parse_ini.hpp

    src/utilities/run_spec.cpp
    src/utilities/run_spec.hpp

    src/utilities/run.cpp
    src/utilities/run.hpp

    src/utilities/event_client.cpp
    src/utilities/event_client.hpp

    src/utilities/utility_classes.cpp
    src/utilities/utility_classes.hpp

)

target_link_libraries(run_grating
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(run_grating PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/")

# checkerboard script
add_executable(run_checkerboard
    src/checkerboard.cpp

    src/utilities/initialize.cpp
    src/utilities/initialize.hpp

    src/utilities/program.cpp
    src/utilities/program.hpp

    src/utilities/msequence.cpp
    src/utilities/msequence.hpp

    src/utilities/parse_ini.cpp
    src/utilities/parse_ini.hpp

    src/utilities/event_client.cpp
    src/utilities/event_client.hpp

    src/utilities/utility_classes.cpp
    src/utilities/utility_classes.hpp

)

target_link_libraries(run_checkerboard
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(run_checkerboard PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/")


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

add_custom_command(
   TARGET run_grating POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/bin"
   COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/run_grating${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/bin/run_grating${CMAKE_EXECUTABLE_SUFFIX}"
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/grating" "${CMAKE_CURRENT_SOURCE_DIR}/bin/grating"
)

add_custom_command(
   TARGET run_checkerboard POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/bin"
   COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/run_checkerboard${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/bin/run_checkerboard${CMAKE_EXECUTABLE_SUFFIX}"
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/checkerboard" "${CMAKE_CURRENT_SOURCE_DIR}/bin/checkerboard"
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/src/m-sequence.bin" "${CMAKE_CURRENT_SOURCE_DIR}/bin/m-sequence.bin"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
