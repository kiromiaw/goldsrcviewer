cmake_minimum_required(VERSION 3.16)
project(vulkan_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(app main.cpp)

target_link_libraries(app
    Vulkan::Vulkan
    glfw
)