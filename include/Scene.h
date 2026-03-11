#pragma once

#include <SDL.h>
#include <SDL_events.h>

class Scene {
 public:
  Scene() = default;
  virtual ~Scene() = default;
  virtual void init() = 0;
  virtual void update(float time) = 0;
  virtual void render() = 0;
  virtual void clean() = 0;
  virtual void handleEvent(SDL_Event *event) = 0;
};
