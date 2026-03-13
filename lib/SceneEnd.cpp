/*
@File    :   lib\SceneEnd.cpp
@Time    :   2026/03/13 21:58:12
@Author  :   loskyertt
@Github  :   https://github.com/loskyertt
@Desc    :   .....
*/

#include "SceneEnd.h"
#include "Game.h"

#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_rect.h>
#include <SDL_scancode.h>

#include <string>

void SceneEnd::init() {
  // 检查是否启用了 Unicode 文本输入事件：如果启用了文本输入事件，则返回 SDL_TRUE，否则返回 SDL_FALSE
  if (!SDL_IsTextInputActive()) {
    SDL_StartTextInput();  // 启动输入模式
    if (!SDL_IsTextInputActive()) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL", "Failed to start text input: %s", SDL_GetError());
    }
  }
}

void SceneEnd::update(float time) {}

void SceneEnd::render() {
  if (isTyping) {
    randerPhase1();
  } else {
    renderPhase2();
  }
}

void SceneEnd::clean() {}

void SceneEnd::handleEvent(SDL_Event *event) {
  if (isTyping) {
    if (event->type == SDL_TEXTINPUT) {
      name += event->text.text;
    }

    if (event->type == SDL_KEYDOWN) {
      // Enter 键：保存退出 -> 切换下一个场景
      if (event->key.keysym.scancode == SDL_SCANCODE_RETURN) {
        if (name == "") {
          name += "没有名字的玩家";  // 玩家未输入名字时的 default name
        }
        isTyping = false;
        SDL_StopTextInput();
      }
      if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
        removeLastUTF8Char();
      }
    }
  } else {
    // TODO
  }
}

/* 渲染文字：第一阶段 */
void SceneEnd::randerPhase1() {
  int score = game.getFinalScore();
  std::string scoreText = "你的得分是：" + std::to_string(score);
  std::string gameOverText = "Game Over!";
  std::string instructionText = "请输入你的名字，按 Enter 键确认：";

  game.renderTextCentered(scoreText, 0.2f, false);
  game.renderTextCentered(gameOverText, 0.4f, true);
  game.renderTextCentered(instructionText, 0.6f, false);

  if (name != "") {
    SDL_Point textCoordinate = game.renderTextCentered(name, 0.8f, false);
  }else {
    game.renderTextCentered("_", 0.8f, false);
  }
}

/* 渲染文字：第二阶段 */
void SceneEnd::renderPhase2() {}

/*
* 移除字符（需要明白英文字符和中文字符的区别）
* 说明：对于 UTF-8
* - 英文字符的二进制编码：占一个字节，左边第一位是 0
* - 中文字符的二进制编码：一般占多个字节，左边开始第一字节的前两位是 11，其后均是以 01 开头
*/
void SceneEnd::removeLastUTF8Char() {
  while (!name.empty()) {
    char lastChar = name.back();
    name.pop_back();  // 先把最后一个字符删除
    // 如果是 ASCII (0xxx xxxx) 或多字节首字节 (11xx xxxx)，停止
    if ((lastChar & 0b11000000) != 0b10000000) {
      break;
    }
  }
}
