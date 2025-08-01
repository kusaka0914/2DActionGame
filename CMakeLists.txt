cmake_minimum_required(VERSION 3.16)
project(2DGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# SDL_mixerを任意にする（見つからなくてもエラーにしない）
find_package(SDL2_mixer QUIET)

# Include directories
include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
if(SDL2_mixer_FOUND)
    message(STATUS "SDL2_mixer found - Sound enabled")
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SOUND_ENABLED)
else()
    message(STATUS "SDL2_mixer not found - Sound disabled")
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)
endif()

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}) 

# Create sounds directory in assets if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets/sounds) 