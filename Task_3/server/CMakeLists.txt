cmake_minimum_required(VERSION 3.17)

project(server LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server main.cpp
                      glm/glm.hpp
                      glad/glad.h
                      glad/glad.c
                      KHR/khrplatform.h
                      shader_class.h)

if(WIN32)
    target_compile_definitions(server PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    find_package(sdl2 REQUIRED)
    target_include_directories(server PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(server   mingw32
                                   SDL2main
                                   SDL2
                                   -mwindows
                                   opengl32)
elseif(UNIX)
    target_link_libraries(server   SDL2
                                        GL)
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(server PRIVATE SDL2::SDL2 SDL2::SDL2main
                                         opengl32)
endif()
