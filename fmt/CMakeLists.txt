add_library(fmt STATIC format.cc os.cc)
target_include_directories(fmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
