cmake_minimum_required(VERSION 3.10)

# Set the default CMAKE_BUILD_TYPE to Release.
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

# project name
project(BoschExercise 
        VERSION 0.1.0
        LANGUAGES CXX)

# specify the c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)
add_subdirectory(test)

# test executable
add_executable(BoschExercise main.cpp)
target_link_libraries(BoschExercise queue)

enable_testing()
