cmake_minimum_required (VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-D_DEBUG -Wall")

project(Pleascach)

if(WIN32)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
endif()

file(GLOB SOURCES
  pléascach.cpp
  */*.cpp
)

add_executable (pleascach ${SOURCES})

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

if(UNIX)
	set(GLFW3_LIBRARY glfw)
endif()


include_directories(${CMAKE_CURRENT_LIST_DIR} include ${GLFW3_INCLUDE_DIR})

target_link_libraries(pleascach ${GLFW3_LIBRARY} Vulkan::Vulkan)

