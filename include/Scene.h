#pragma once

#include <SDL.h>
#include <SDL_events.h>

// 避免头文件循环引用，这里只做声明，在 Scene.cpp 中 include Game.h
class Game;

class Scene {
 protected:
  Game &game;

 public:
  Scene();

  virtual ~Scene() = default;
  virtual void init() = 0;
  virtual void update(float time) = 0;
  virtual void render() = 0;
  virtual void clean() = 0;
  virtual void handleEvent(SDL_Event *event) = 0;
};
