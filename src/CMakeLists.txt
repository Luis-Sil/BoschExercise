add_library(queue STATIC queue.h queue.cpp)
target_include_directories(queue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
