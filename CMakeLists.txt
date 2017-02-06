cmake_minimum_required(VERSION 2.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

# Directories
set(BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(DEPENDENCIES_DIR ${CMAKE_CURRENT_LIST_DIR}/dependencies/)
set(DEMO_SYSTEM_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/system/public/)

# Set output directories
set(ARCHIVE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ARCHIVE_OUTPUT_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ARCHIVE_OUTPUT_DIR} )

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${RUNTIME_OUTPUT_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${ARCHIVE_OUTPUT_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${ARCHIVE_OUTPUT_DIR} )
	message("CONFIG_SET = " ${OUTPUTCONFIG})
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# Load modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

# Find DirectX 
if(WIN32)
	find_package(DirectX REQUIRED)
endif(WIN32)

# Find OpenGL
find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )

# Find SDL
find_package(CustomSDL REQUIRED)

# Set include directories shared by all projects
include_directories(${DEPENDENCIES_DIR}/GLM/)
include_directories(${DEPENDENCIES_DIR}/SDL/include/)

# Create Solution
project(GraphicsDemos CXX C)

# Add 'Demo System' Library
add_subdirectory(system)

# Add all demo executables
add_subdirectory(demos)

