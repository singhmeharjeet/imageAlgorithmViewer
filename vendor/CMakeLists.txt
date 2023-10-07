cmake_minimum_required(VERSION 3.5)
project(image)



# SDL3 Flags
set(SDL_TESTS OFF)
set(SDL_TEST_LIBRARY OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
add_subdirectory(SDL)


#SDL_Image
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(BUILD_SHARED_LIBS OFF)
set(SDL_IMAGE_TIF ON)
add_subdirectory(SDL_image)

# TinyFileDialogs Flags
set(TFD_BUILD_STATIC ON)
add_subdirectory(tinyfiledialogs-cmake)


#IMGUI
add_subdirectory(imgui)


add_library(vendor INTERFACE)

target_link_libraries(vendor 
	INTERFACE 
		SDL3::SDL3
		SDL3_image::SDL3_image
		imgui
		tfd
)