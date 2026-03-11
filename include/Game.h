#pragma once

#include "Scene.h"

#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>

class Game {
 private:
  Scene *currentScene = nullptr;
  bool isRunning = true;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

 private:
  int windowWidth = 600;
  int windowHeight = 800;
  int FPS = 60;
  Uint32 frameTime;  // 标准：1000/60 ms
  float deltaTime;   // 实际每帧时间，单位：s

 private:
  Game();
  // 删除拷贝与赋值构造函数
  Game(const Game &) = delete;
  Game &operator=(const Game &) = delete;

 public:
  static Game &getInstance();
  ~Game();

 public:
  void run();                          // 运行
  void init();                         // 初始化
  void clean();                        // 清理
  void changeScene(Scene *scene);      // 切换场景
  void handleEvent(SDL_Event *event);  // 事件处理
  void update(float time);             // 逻辑更新
  void render();                       // 渲染绘制

 public:
  SDL_Window *getWindow() { return window; }  // 获取窗口

  SDL_Renderer *getRenderer() { return renderer; }  // 获取渲染器

  int getWindowWidth() { return windowWidth; }  // 获取窗口宽度

  int getWindowHeight() { return windowHeight; }  // 获取窗口高度
};
