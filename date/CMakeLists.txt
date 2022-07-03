add_library(date STATIC tz.cpp)
target_include_directories(date PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
