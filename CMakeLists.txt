# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (OLG_EX)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
#     message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
# endif()
# if( CMAKE_SOURCE_DIR MATCHES " " )
# 	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
# endif()
# if( CMAKE_BINARY_DIR MATCHES " " )
# 	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
# endif()
#


# Compile external dependencies
add_subdirectory(external)

# On Visual 2005 and above, this module can set the debug working directory
# cmake_policy(SET CMP0026 OLD)
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
# include(CreateLaunchers)
# include(MSVCMultipleProcessCompile) # /MP

# if(INCLUDE_DISTRIB)
# 	add_subdirectory(distrib)
# endif(INCLUDE_DISTRIB)



include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
    # external/AntTweakBar-1.16/include/
	# external/assimp-3.0.1270/include/
	# external/bullet-2.81-rev2613/src/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# example script
add_executable(ogl_ex
    src/main.cpp

    util/initialize.cpp
    util/initialize.hpp

    util/bmp.cpp
    util/bmp.hpp

    util/program.cpp
    util/program.hpp

    common/controls.cpp
    common/controls.hpp

    src/05/fragment.glsl
    src/05/vertex.glsl
)

target_link_libraries(ogl_ex
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(ogl_ex PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
# create_target_launcher(ogl WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ogl_ex/")

# plot script
add_executable(plot_ex
    src/plot.cpp

    util/initialize.cpp
    util/initialize.hpp

    util/program.cpp
    util/program.hpp

    # common/controls.cpp
    # common/controls.hpp

    src/plot/fragment.glsl
    src/plot/vertex.glsl

    src/viewport.hpp
)

target_link_libraries(plot_ex
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(plot_ex PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
# create_target_launcher(ogl WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/plot_ex/")

# grating script
add_executable(grating_ex
    src/grating.cpp

    util/initialize.cpp
    util/initialize.hpp

    util/program.cpp
    util/program.hpp

    util/parse_ini.cpp
    util/parse_ini.hpp

    util/run_spec.cpp
    util/run_spec.hpp

    util/run.cpp
    util/run.hpp

    util/event_client.cpp
    util/event_client.hpp

    util/utility_classes.cpp
    util/utility_classes.hpp

    src/grating/fragment.glsl
    src/grating/vertex.glsl

)

target_link_libraries(grating_ex
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(grating_ex PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
# create_target_launcher(ogl WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/plot_ex/")

# checkerboard script
add_executable(checkerboard_ex
    src/checkerboard.cpp

    util/initialize.cpp
    util/initialize.hpp

    util/program.cpp
    util/program.hpp

    util/msequence.cpp
    util/msequence.hpp

    util/parse_ini.cpp
    util/parse_ini.hpp

    util/event_client.cpp
    util/event_client.hpp

    util/utility_classes.cpp
    util/utility_classes.hpp

    # src/checkerboard/fragment.glsl
    # src/checkerboard/vertex.glsl
)

target_link_libraries(checkerboard_ex
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(checkerboard_ex PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

add_custom_command(
   TARGET ogl_ex POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ogl_ex${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

add_custom_command(
   TARGET plot_ex POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/plot_ex${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

add_custom_command(
   TARGET grating_ex POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/grating_ex${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

add_custom_command(
   TARGET checkerboard_ex POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/checkerboard_ex${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
