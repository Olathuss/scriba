cmake_minimum_required(VERSION 3.20)
project(scriba LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SCRIBA_HEADERS "include/scriba/*.h")
file(GLOB_RECURSE SCRIBA_SOURCES "src/*.cpp")

add_library(scriba STATIC
    ${SCRIBA_HEADERS}
    ${SCRIBA_SOURCES}
)

target_include_directories(scriba PUBLIC include)

add_subdirectory(examples)