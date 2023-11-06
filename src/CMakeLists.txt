cmake_minimum_required(VERSION 3.5)
project(image)

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        "${CMAKE_HOME_DIRECTORY}/include"
        "${CMAKE_HOME_DIRECTORY}/vendor/SDL/include"
        "${CMAKE_HOME_DIRECTORY}/vendor/SDL_image/include"
        "${CMAKE_HOME_DIRECTORY}/vendor/imgui"
        "${CMAKE_HOME_DIRECTORY}/vendor/imgui/backends"
        "${CMAKE_HOME_DIRECTORY}/vendor/tinyfiledialogs-cmake/include"
        "${CMAKE_HOME_DIRECTORY}/vendor/TinyTIFF/src"
)
target_sources(${PROJECT_NAME}
        PRIVATE
        image.h
        main.cpp
        myfont.cpp
        style.h
)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        vendor
)