cmake_minimum_required (VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
set(CMAKE_CXX_FLAGS "/O2")
else()
set(CMAKE_CXX_FLAGS "-D_DEBUG -Wall")
endif()

project(Pleascach)

if(WIN32)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
endif()

file(GLOB SOURCES
  pléascach.cpp
  */*.cpp
  include/imgui/*.cpp
)

add_executable (pleascach ${SOURCES})

# shader compilation
file(GLOB SHADER_SOURCE_FILES
	assets/shaders/**/*.frag
	assets/shaders/**/*.vert
	assets/shaders/**/*.geom
	assets/shaders/**/*.tese
	assets/shaders/**/*.tesc
	assets/shaders/*/*/*.frag
	assets/shaders/*/*/*.vert
	assets/shaders/*/*/*.geom
	assets/shaders/*/*/*.tese
	assets/shaders/*/*/*.tesc
)

foreach(SHADER_SOURCE ${SHADER_SOURCE_FILES})
	message("Processing shader file")
	get_filename_component(FILE_NAME ${SHADER_SOURCE} NAME)
	set(SPIRV "${CMAKE_SOURCE_DIR}/assets/shaders/bin/${FILE_NAME}.spv")
	add_custom_command(
		OUTPUT ${SPIRV}
		COMMAND glslc -o ${SPIRV} ${SHADER_SOURCE}
		DEPENDS ${SHADER_SOURCE}
	)
	list(APPEND SPIRV_BIN_FILES ${SPIRV})
endforeach(SHADER_SOURCE)

add_custom_target (
	shaders
	DEPENDS ${SPIRV_BIN_FILES}
)

add_dependencies(pleascach shaders)

# copy assets somewhere accessible by the accessable
if(UNIX)
	add_custom_command(
		TARGET shaders POST_BUILD
		COMMAND cmake -E copy_directory
		"$<TARGET_PROPERTY:pleascach,SOURCE_DIR>/assets"
		"$<TARGET_PROPERTY:pleascach,BINARY_DIR>/assets")
else()
	add_custom_command(
		TARGET shaders POST_BUILD
		COMMAND cmake -E copy_directory
		"$<TARGET_PROPERTY:pleascach,SOURCE_DIR>/assets"
		"$<TARGET_PROPERTY:pleascach,BINARY_DIR>/assets")
endif()


find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

if(UNIX)
	set(GLFW3_LIBRARY glfw)
endif()


include_directories(${CMAKE_CURRENT_LIST_DIR} include ${GLFW3_INCLUDE_DIR})

target_link_libraries(pleascach ${GLFW3_LIBRARY} Vulkan::Vulkan)

