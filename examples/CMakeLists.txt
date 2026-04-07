# examples/CMakeLists.txt

file(GLOB_RECURSE ALL_SRCS *.cpp)
foreach(v ${ALL_SRCS})
  message("Processing: ${v}")
  get_filename_component(TARGET ${v} NAME_WE) # 生成目标名（去掉路径和扩展名）

  add_executable(${TARGET} ${v})
  target_link_libraries(${TARGET}
    PUBLIC
      ${SDL2_LIBRARIES}
      SDL2_image::SDL2_image
      SDL2_mixer::SDL2_mixer
      SDL2_ttf::SDL2_ttf
  )
endforeach()

message(STATUS "--------------------------------------------\n")
