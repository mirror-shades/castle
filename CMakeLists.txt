cmake_minimum_required(VERSION 3.10)
project(main)

# Set build type to Release
set(CMAKE_BUILD_TYPE Release)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set SFML directory based on platform
if(APPLE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        set(SFML_DIR "${CMAKE_SOURCE_DIR}/external/SFML-Mac-ARM64/lib/cmake/SFML")
    else()
        set(SFML_DIR "${CMAKE_SOURCE_DIR}/external/SFML-Mac/lib/cmake/SFML")
    endif()
elseif(WIN32)
    set(SFML_DIR "${CMAKE_SOURCE_DIR}/external/SFML-Windows/lib/cmake/SFML")
else()
    set(SFML_DIR "${CMAKE_SOURCE_DIR}/external/SFML-Linux/lib/cmake/SFML")
endif()

# Find SFML packages
find_package(SFML 2.6.1 COMPONENTS system window graphics audio REQUIRED)

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp src/render.cpp src/buttons.cpp src/core.cpp src/levels.cpp)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    sfml-system
    sfml-window
    sfml-graphics
)

# Copy SFML DLLs/dylibs to output directory
if(WIN32)
    # Windows: Copy DLLs from SFML Windows directory
    file(COPY "${CMAKE_SOURCE_DIR}/external/SFML-Windows/bin/sfml-system-2.dll" DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY "${CMAKE_SOURCE_DIR}/external/SFML-Windows/bin/sfml-graphics-2.dll" DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY "${CMAKE_SOURCE_DIR}/external/SFML-Windows/bin/sfml-window-2.dll" DESTINATION ${CMAKE_BINARY_DIR})
elseif(APPLE)
    # macOS: Copy dylibs from appropriate Mac directory
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        set(SFML_BIN_DIR "${CMAKE_SOURCE_DIR}/external/SFML-Mac-ARM64/lib")
    else()
        set(SFML_BIN_DIR "${CMAKE_SOURCE_DIR}/external/SFML-Mac/lib")
    endif()
    file(COPY "${SFML_BIN_DIR}/libsfml-system.2.6.1.dylib" DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY "${SFML_BIN_DIR}/libsfml-graphics.2.6.1.dylib" DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY "${SFML_BIN_DIR}/libsfml-window.2.6.1.dylib" DESTINATION ${CMAKE_BINARY_DIR})
endif()

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${SFML_DIR}/../../include
)
