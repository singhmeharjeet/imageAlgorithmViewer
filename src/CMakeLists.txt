cmake_minimum_required(VERSION 3.5)
project(image)


add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} 
	PRIVATE 
	${CMAKE_HOME_DIRECTORY}/include
	"${CMAKE_HOME_DIRECTORY}/vendor/tinyfiledialogs-cmake/include"
	"${CMAKE_HOME_DIRECTORY}/vendor/SDL/include"
	"${CMAKE_HOME_DIRECTORY}/vendor/SDL_image/include"
	"${CMAKE_HOME_DIRECTORY}/vendor/imgui"
	"${CMAKE_HOME_DIRECTORY}/vendor/TinyTIFF/src"
)
target_sources(${PROJECT_NAME} 
	PRIVATE 
		main.cpp
		myfont.cpp
		# "${CMAKE_HOME_DIRECTORY}/vendor/imgui"
		# "${CMAKE_HOME_DIRECTORY}/vendor/tinyfiledialogs-cmake/src/tinyfiledialogs.c"
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE
		# ${CMAKE_HOME_DIRECTORY}/vendor
		# ${CMAKE_HOME_DIRECTORY}/vendor/imgui
		# ${CMAKE_HOME_DIRECTORY}/vendor/TinyTIFF
		# "${CMAKE_HOME_DIRECTORY}/vendor/SDL3-static"
		# "${CMAKE_HOME_DIRECTORY}/vendor/tfd"
		vendor
		# SDL3::SDL3
		# imgui
		# TinyTIFF
)