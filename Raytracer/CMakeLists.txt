cmake_minimum_required(VERSION 3.2)

project(ray)


find_package(Boost COMPONENTS mpi serialization REQUIRED)
find_package(MPI REQUIRED)

# Create a debug build
set(CMAKE_CXX_FLAGS "-Wall --std=c++17 -fopenmp -march=native")

# Set all CPP files to be source files
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Code/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

include_directories(Code/)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${MPI_LIBRARIES})
