# for each "lib/x.cpp", generate shared library or static library
message("CMakeLists.txt for lib directory")

# 在这里添加动态库
# - 动态库：gamedll
add_library(gamedll SHARED
  Game.cpp
  Scene.cpp
  SceneMain.cpp
  SceneTitle.cpp
  SceneEnd.cpp
)

target_include_directories(gamedll PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(gamedll
  PUBLIC
    ${SDL2_LIBRARIES}
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
)

if(MSVC)
  set_target_properties(gamedll PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON) # 单独为 gamedll 启用 Windows 下的符号自动导出功能
  target_compile_options(gamedll PRIVATE /utf-8)  # UTF-8 支持
endif()

message(STATUS "--------------------------------------------\n")
