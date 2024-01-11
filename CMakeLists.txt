cmake_minimum_required (VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Pleascach)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

file(GLOB SOURCES
  pléascach.cpp
  */*.cpp
)

add_executable (Pleascach ${SOURCES} "vkb/VkBootstrap.cpp" "vma/vma.cpp")

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

if(UNIX)
	set(GLFW3_LIBRARY glfw)
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR} include ${GLFW3_INCLUDE_DIR})

target_link_libraries(Pleascach ${GLFW3_LIBRARY} Vulkan::Vulkan)

