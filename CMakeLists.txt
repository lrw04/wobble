cmake_minimum_required(VERSION 3.9)
project(wobble)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(argagg)
add_subdirectory(date)
add_subdirectory(fmt)
add_subdirectory(json)
add_subdirectory(loguru)
add_subdirectory(tiny-process-library)

add_executable(wobble config.cpp job.cpp report.cpp schedule.cpp util.cpp wobble.cpp)
target_link_libraries(wobble argagg date fmt json loguru tiny-process-library)

# The following is unnecessary as CMake will link ole32 automatically
# if(WIN32)
#     target_link_libraries(wobble ole32)
# endif(WIN32)

if(UNIX)
    target_link_libraries(wobble dl)
endif(UNIX)
