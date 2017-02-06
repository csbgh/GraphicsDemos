# SOIL image library
#add_library(SOIL_LIB SHARED IMPORTED)
#set_property(TARGET SOIL_LIB PROPERTY IMPORTED_IMPLIB ${DEPENDENCIES_DIR}/SOIL/lib/windows/SOILRelease.lib)

# Add SOIL
#target_link_libraries(DemoSystem SOIL_LIB)
#target_include_directories(DemoSystem PUBLIC ${DEPENDENCIES_DIR}/SOIL/include)

# Demo System Library
file(GLOB DEMO_SYSTEM_PUBLIC_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/public/*.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/public/*.h")
source_group("public" FILES ${DEMO_SYSTEM_PUBLIC_SOURCES})

file(GLOB DEMO_SYSTEM_PRIVATE_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/private/*.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/private/*.h")
source_group("private" FILES ${DEMO_SYSTEM_PRIVATE_SOURCES})

file(GLOB IM_GUI_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/public/imgui/*.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/public/imgui/*.h")
source_group("public\\imgui" FILES ${IM_GUI_SOURCES})

file(GLOB GLEW_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/lib/GL/*.c"
	"${CMAKE_CURRENT_LIST_DIR}/lib/GL/*.h")
source_group("lib\\glew" FILES ${GLEW_SOURCES})

file(GLOB STB_IMAGE_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/lib/*.h")
source_group("lib" FILES ${STB_IMAGE_SOURCES})

file(GLOB DS_UTILITY_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/public/utility/*.h")
source_group("public\\utility" FILES ${DS_UTILITY_SOURCES})
	
add_library(DemoSystem STATIC
			${DEMO_SYSTEM_PRIVATE_SOURCES} 
			${DEMO_SYSTEM_PUBLIC_SOURCES} 
			${IM_GUI_SOURCES} 
			${GLEW_SOURCES}
			${STB_IMAGE_SOURCES}
			${DS_UTILITY_SOURCES})

set_target_properties(DemoSystem PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET DemoSystem PROPERTY FOLDER "System")

if(WIN32)
set_property(TARGET DemoSystem PROPERTY COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS GLEW_STATIC)
endif(WIN32)

target_include_directories(DemoSystem PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/public/)
target_include_directories(DemoSystem PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private/)
target_include_directories(DemoSystem PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib/)

target_include_directories(DemoSystem PRIVATE ${DEPENDENCIES_DIR}/SOIL/include)
