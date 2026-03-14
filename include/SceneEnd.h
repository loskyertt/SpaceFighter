/*
@File    :   include\SceneEnd.h
@Time    :   2026/03/13 21:58:18
@Author  :   loskyertt
@Github  :   https://github.com/loskyertt
@Desc    :   .....
*/

#pragma once

#include <SDL_mixer.h>
#include <string>
#include "Scene.h"

class SceneEnd : public Scene {
 private:
  Mix_Music *bgm;  // 结算场景音乐
  bool isTyping = true;
  std::string name = "";    // 玩家姓名
  float blinkTimer = 1.0f;  // 下划线光标闪烁频率

  /* 接口实现 */
 public:
  void init() override;
  void update(float time) override;
  void render() override;
  void clean() override;
  void handleEvent(SDL_Event *event) override;

 private:
  void randerPhase1();  // 渲染文字：第一阶段
  void renderPhase2();  // 渲染文字：第二阶段

  /*
  * 说明：对于 UTF-8
  * - 英文字符的二进制编码：占一个字节，左边第一位是 0
  * - 中文字符的二进制编码：一般占多个字节，左边开始第一字节的前两位是 11，其后均是以 01 开头
  */
  void removeLastUTF8Char();  // 移除字符（需要明白英文字符和中文字符的区别）
};
