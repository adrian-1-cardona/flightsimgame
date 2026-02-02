cmake_minimum_required(VERSION 3.16)
project(FlightSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Renderer.cpp
    src/Aircraft.cpp
    src/InputManager.cpp
    src/MenuSystem.cpp
    src/AudioManager.cpp
    src/SettingsManager.cpp
    src/Terrain.cpp
    src/Sky.cpp
    src/LoadingScreen.cpp
    src/Camera.cpp
    src/Physics.cpp
)

# Header files
set(HEADERS
    include/Game.h
    include/Renderer.h
    include/Aircraft.h
    include/InputManager.h
    include/MenuSystem.h
    include/AudioManager.h
    include/SettingsManager.h
    include/Terrain.h
    include/Sky.h
    include/LoadingScreen.h
    include/Camera.h
    include/Physics.h
    include/Types.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    "-framework CoreFoundation"
    "-framework IOKit"
    "-framework CoreAudio"
    "-framework AudioToolbox"
)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
