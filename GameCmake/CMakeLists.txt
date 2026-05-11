cmake_minimum_required(VERSION 3.15)
project(Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-3.0.0/lib/cmake/SFML")
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(Game src/main.cpp)

target_link_libraries(Game SFML::Graphics)

if(WIN32)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-3.0.0/bin/" 
         DESTINATION "${CMAKE_BINARY_DIR}")
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data" DESTINATION "${CMAKE_BINARY_DIR}")
