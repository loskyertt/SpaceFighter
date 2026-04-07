cmake_minimum_required(VERSION 3.20.0)
project(SpaceFighter VERSION 0.0.1 LANGUAGES C CXX)

# 设置标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# 导出编译命令（用于 clangd）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
  add_compile_options(/utf-8)
endif()

# 开启 gtest_discover_tests 注册测试用例依赖
# enable_testing()

# 脚本
# include(cmake/checkArch.cmake)

if(MSVC)
  list(APPEND
    CMAKE_PREFIX_PATH
      "${CMAKE_SOURCE_DIR}/3rdparty/SDL2-2.32.10/cmake"
      "${CMAKE_SOURCE_DIR}/3rdparty/SDL2_image-2.8.4/cmake"
      "${CMAKE_SOURCE_DIR}/3rdparty/SDL2_mixer-2.8.1/cmake"
      "${CMAKE_SOURCE_DIR}/3rdparty/SDL2_ttf-2.24.0/cmake"
  )
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

# 子模块
add_subdirectory(src)
add_subdirectory(examples)
