# for all "src/x.cpp", generate target "x"
message("CMakeLists.txt for src directory")

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

add_executable(${TARGET}
  main.cpp
)

target_link_libraries(${TARGET}
  PRIVATE
    gamedll
)

if(MSVC)
  target_compile_options(${TARGET} PRIVATE /utf-8)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # Release 模式：不显示控制台窗口，直接运行游戏
  # /ENTRY:mainCRTStartup 告诉链接器使用 Windows 入口点（不需要控制台）
  set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
else()
  # Debug 模式（默认）：显示控制台窗口，方便调试
  set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

message(STATUS "--------------------------------------------\n")
