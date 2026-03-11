#pragma once

#include "Scene.h"

#pragma once

class SceneMain : public Scene {
 public:
  SceneMain() = default;
  ~SceneMain() override = default;
  void init() override;
  void update() override;
  void render() override;
  void clean() override;
  void handleEvent(SDL_Event *event) override;
};
