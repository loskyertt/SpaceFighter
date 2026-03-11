#pragma once

#include "Scene.h"

#include <SDL_render.h>

class Game {
 private:
  Scene *currentScene = nullptr;
  bool isRunning = true;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

 private:
  int windowWidth = 600;
  int windowHeight = 800;

 public:
  Game();
  ~Game();
  void run();                      // 运行
  void init();                     // 初始化
  void clean();                    // 清理
  void changeScene(Scene *scene);  // 切换场景
  void handleEvent(SDL_Event *event);
  void update();
  void render();
};
