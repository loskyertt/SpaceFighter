/*
@File    :   lib\SceneTitle.cpp
@Time    :   2026/03/13 17:13:21
@Author  :   loskyertt
@Github  :   https://github.com/loskyertt
@Desc    :   .....
*/

#include "SceneTitle.h"
#include "Game.h"
#include "SceneMain.h"

#include <SDL_events.h>
#include <SDL_mixer.h>
#include <SDL_scancode.h>

#include <string>

SceneTitle::SceneTitle() {}

SceneTitle::~SceneTitle() {}

void SceneTitle::init() {
  // 载入并播放背景音乐
  bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
  if (!bgm) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load music: %s\n", Mix_GetError());
  }
  Mix_PlayMusic(bgm, -1);
}

void SceneTitle::update(float time) {
  timer += time;
  if (timer > 1.0f) {
    timer -= 1.0f;
  }
}

void SceneTitle::render() {
  // 渲染标题文字
  std::string titleText = "SDL 太空战机";
  game.renderTextCentered(titleText, 0.4f, true);

  // 渲染文本文字：实现闪烁效果
  if (timer < 0.5f) {
    std::string instructions = "按 J 键开始游戏";
    game.renderTextCentered(instructions, 0.8f, false);
  }
}

void SceneTitle::clean() {}

void SceneTitle::handleEvent(SDL_Event *event) {
  if (event->type == SDL_KEYDOWN) {
    if (event->key.keysym.scancode == SDL_SCANCODE_J) {
      Scene *sceneMain = new SceneMain();
      game.changeScene(sceneMain);
    }
  }
}
