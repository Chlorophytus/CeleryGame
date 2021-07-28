# Minimum CMake version is 3.16
cmake_minimum_required(VERSION 3.16)
# We are at v0.4.0
project(celerygame VERSION 0.4.0.10)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# Configure the project header(s)
configure_file(include/${PROJECT_NAME}_cfg.in
	${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}_cfg.hpp
)
# Find SDL2 
find_package(SDL2 CONFIG REQUIRED)
# Find Vulkan
find_package(Vulkan REQUIRED)
# Find LuaJIT
find_package(LuaJIT REQUIRED)
# Find pthread or other platform threading library
find_package(Threads REQUIRED)
# Find GLM
find_package(glm REQUIRED)
# Find Doxygen
find_package(Doxygen)
# Add target
add_executable(${PROJECT_NAME}
	src/${PROJECT_NAME}_console.cpp
	src/${PROJECT_NAME}_runloop.cpp
	src/${PROJECT_NAME}_lua.cpp
	src/${PROJECT_NAME}_vulkan_getset.cpp
	src/${PROJECT_NAME}_vulkan_instance.cpp
	src/${PROJECT_NAME}_vulkan_window.cpp
	src/${PROJECT_NAME}_vulkan_utils.cpp
	src/${PROJECT_NAME}.cpp
)
# Generate docs
doxygen_add_docs(docs)
# MSVC doesn't like post-C99 extensions
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
# Enable C++17 though
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Include and make aware GLEW, Lua, SDL2 headers...
target_include_directories(${PROJECT_NAME} PRIVATE include
	${LUA_INCLUDE_DIR}
)
# Time to link
target_link_libraries(${PROJECT_NAME} PRIVATE
	Vulkan::Vulkan
	SDL2::SDL2main
	SDL2::SDL2
	Threads::Threads
	${GLM_LIBRARIES}
	${LUA_LIBRARY}
)
