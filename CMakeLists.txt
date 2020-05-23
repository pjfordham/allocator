cmake_minimum_required(VERSION 3.10)

# set the project name
project(Allocator VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(Allocator allocator.cc)

list(APPEND EXTRA_LIBS fmt)
target_link_libraries(Allocator PUBLIC ${EXTRA_LIBS})
