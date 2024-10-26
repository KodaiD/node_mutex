cmake_minimum_required(VERSION 3.14)
project(node-mutex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# For normal build
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")

# For release build
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# For debug build
set(CMAKE_CXX_FLAGS_DEBUG "-fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=leak")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-sanitize-recover=address")

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE include)
target_link_libraries(main PRIVATE pthread)