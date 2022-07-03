add_library(tiny-process-library STATIC process_platform.cpp process.cpp)
target_include_directories(tiny-process-library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
