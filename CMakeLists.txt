cmake_minimum_required(VERSION 3.10)
project(ParkingJam3D)

set(CMAKE_CXX_STANDARD 17)

# Add glad
add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)

# Main executable
add_executable(ParkingJam3D src/main.cpp)

# Link GLFW + OpenGL + GLAD
target_link_libraries(ParkingJam3D 
    glad
    glfw3
    opengl32
)
