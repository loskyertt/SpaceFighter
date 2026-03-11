# for all "src/x.cpp", generate target "x"
message("CMakeLists.txt for src directory")

set(ALL_SRCS
  main.cpp
)

add_executable(my_program ${ALL_SRCS})

# target_include_directories(my_program PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(my_program
  PRIVATE
    gamedll
)

target_compile_options(my_program PRIVATE /utf-8)

message(STATUS "--------------------------------------------\n")
