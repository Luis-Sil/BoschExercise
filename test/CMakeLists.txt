enable_testing()

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain PUBLIC queue)

include(Catch)
catch_discover_tests(tests)