/*
@File    :   include\SceneTitle.h
@Time    :   2026/03/13 17:11:26
@Author  :   loskyertt
@Blog    :   https://github.com/loskyertt
@Desc    :   .....
*/

#pragma once

#include "Scene.h"

#include <SDL_mixer.h>

class SceneTitle : public Scene {
 private:
  Mix_Music *bgm;  // 背景音乐
  float timer = 0;     // 计时器

 public:
  SceneTitle();
  ~SceneTitle() override;

  /* 接口实现 */
 public:
  void init() override;
  void update(float time) override;
  void render() override;
  void clean() override;
  void handleEvent(SDL_Event *event) override;
};
