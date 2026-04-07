# src/CMakeLists.txt
set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

# core 模块：编译为动态库
add_library(core SHARED
  core/impl/Game.cpp
  core/impl/Scene.cpp
  core/impl/SceneMain.cpp
  core/impl/SceneTitle.cpp
  core/impl/SceneEnd.cpp
)
target_include_directories(core
  PUBLIC  core/include
  PRIVATE core/impl
)
target_link_libraries(core
  PUBLIC
    ${SDL2_LIBRARIES}
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
)

if(MSVC)
  set_target_properties(core PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# 主可执行文件
add_executable(${TARGET} main.cpp)
target_link_libraries(${TARGET} PRIVATE core)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # Release 模式：不显示控制台窗口，直接运行游戏
  # /ENTRY:mainCRTStartup 告诉链接器使用 Windows 入口点（不需要控制台）
  set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
else()
  # Debug 模式（默认）：显示控制台窗口，方便调试
  set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()
