#set (demo_sources_var DEMO_${demo_name}_SOURCES)
macro (create_demo_executable number name)

set (demo_name ${number}-${name})
set (demo_full_name Demo-${number}-${name})

file(GLOB DEMO_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/${demo_name}/*.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/${demo_name}/*.h")

source_group("src" FILES ${DEMO_SOURCES})

# Create demo executable project
add_executable(${demo_full_name} ${DEMO_SOURCES})

target_include_directories(${demo_full_name} PUBLIC ${DEMO_SYSTEM_INCLUDE})
add_dependencies(${demo_full_name} DemoSystem)
add_dependencies(${demo_full_name} SDL_IMPORTED_LIB)
target_link_libraries(${demo_full_name} DemoSystem)

set_target_properties(${demo_full_name} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${demo_full_name} PROPERTIES FOLDER "Demos")

if(WIN32)
	set_target_properties(${demo_full_name} PROPERTIES COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS)
endif(WIN32)

#set(SOIL_LIB_DIR ${DEPENDENCIES_DIR}SOIL/lib/windows/)
#target_link_libraries(${demo_full_name} optimized ${SOIL_LIB_DIR}SOILRelease.lib debug ${SOIL_LIB_DIR}SOILDebug.lib)

# Link SDL
target_link_libraries(${demo_full_name} SDL_IMPORTED_LIB)
target_link_libraries(${demo_full_name} SDL_MAIN_IMPORTED_LIB)

# Link OpenGL
target_link_libraries(${demo_full_name} ${OPENGL_LIBRARIES})

# Link DirectX 11
target_link_libraries(${demo_full_name} ${DirectX_D3D11_LIBRARY})
target_link_libraries(${demo_full_name} ${DirectX_D3DCompiler_LIBRARY})

if(MSVC)
	
	# Ensure project output directory doesn't change for different configurations
	set_target_properties( ${demo_full_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR} )
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		set_target_properties( ${demo_full_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${RUNTIME_OUTPUT_DIR} )
		set_target_properties( ${demo_full_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${ARCHIVE_OUTPUT_DIR} )
		set_target_properties( ${demo_full_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${ARCHIVE_OUTPUT_DIR} )
		message("CONFIG_SET = " ${OUTPUTCONFIG})
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
	
	# Add a user file to set the working directory for debugging in Visual Studio
	configure_file(${CMAKE_SOURCE_DIR}/cmake/WorkingDirTemplate.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${demo_full_name}.vcxproj.user @ONLY)

endif(MSVC)

# Add post build command to copy SDL binaries to output directory
add_custom_command (TARGET ${demo_full_name} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
					COMMENT "Copying SDL binaries" VERBATIM)

# Add post build command to copy resources folder to output directory
add_custom_command(TARGET ${demo_full_name} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${BASE_DIR}/resources/ $<TARGET_FILE_DIR:${demo_full_name}>/resources/)
				  
endmacro ()

# Demo 01 : Simple
create_demo_executable(01 Simple)

