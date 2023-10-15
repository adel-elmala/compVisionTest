cmake_minimum_required(VERSION 3.0.0)

project ("CompVisonTest")

set(CMAKE_BUILD_TYPE "Debug")

include_directories(./include)

find_package(imgui CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

set(APP_NAME "CompVisionTest")

#add_library(gui "src/gui.cpp" "include/gui.h")
#target_link_libraries(gui PUBLIC imgui::imgui)

add_executable(${APP_NAME} "main.cpp" src/window.cpp )

target_link_libraries(${APP_NAME} PRIVATE
    imgui::imgui
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    OpenGL::GL
)

