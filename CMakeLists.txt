cmake_minimum_required(VERSION 3.16)
project(2DGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Include directories
include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2_image::SDL2_image)

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}) 