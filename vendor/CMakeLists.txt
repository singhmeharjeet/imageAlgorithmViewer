cmake_minimum_required(VERSION 3.5)
project(image)

# SDL3 Flags
set(SDL_TESTS OFF)
set(SDL_TEST_LIBRARY OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
set(SDL_VULKAN OFF)
set(SDL_RENDER_METAL OFF)
set(SDL_ASSEMBLY OFF)
add_subdirectory(SDL)


#SDL_Image
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(BUILD_SHARED_LIBS OFF)
set(SDL_IMAGE_TIF ON)
add_subdirectory(SDL_image)

# TinyFileDialogs Flags
set(TFD_BUILD_STATIC ON)
add_subdirectory(tinyfiledialogs-cmake)

# ImGui LIB
set(IMGUI_HEADERS 
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/imgui.h"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/imconfig.h"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/imgui_internal.h"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/backends/imgui_impl_sdl3.h"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/backends/imgui_impl_sdlrenderer3.h"
)
set(IMGUI_SOURCES 
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/imgui.cpp"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/imgui_draw.cpp"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/imgui_demo.cpp"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/imgui_tables.cpp"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/imgui_widgets.cpp"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/backends/imgui_impl_sdl3.cpp"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui/backends/imgui_impl_sdlrenderer3.cpp"
)
add_library(imgui ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${CMAKE_HOME_DIRECTORY}/vendor/imgui)
target_include_directories(imgui PUBLIC ${CMAKE_HOME_DIRECTORY}/vendor/SDL/include)
target_link_libraries(imgui PUBLIC SDL3-static)

add_library(vendor INTERFACE)

target_link_libraries(vendor 
	INTERFACE 
		SDL3-static
		SDL3_image-static
		tfd
		imgui
)