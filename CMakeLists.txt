cmake_minimum_required(VERSION 3.9)
project(wobble)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(argagg)
add_subdirectory(date)
FetchContent_Declare(json GIT_REPOSITORY
    https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
    GIT_PROGRESS TRUE GIT_SHALLOW TRUE GIT_TAG v3.10.5)
FetchContent_MakeAvailable(json)
FetchContent_Declare(fmt GIT_REPOSITORY
    https://github.com/fmtlib/fmt
    GIT_PROGRESS TRUE GIT_SHALLOW TRUE GIT_TAG 8.1.1)
FetchContent_MakeAvailable(fmt)
add_subdirectory(loguru)
add_subdirectory(tiny-process-library)

add_executable(wobble config.cpp job.cpp report.cpp schedule.cpp util.cpp wobble.cpp)
target_link_libraries(wobble argagg date fmt::fmt nlohmann_json::nlohmann_json loguru tiny-process-library)

# The following is unnecessary as CMake will link ole32 automatically
# if(WIN32)
# target_link_libraries(wobble ole32)
# endif(WIN32)
if(UNIX)
    target_link_libraries(wobble dl)
endif(UNIX)
