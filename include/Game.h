/*
@File    :   include\Game.h
@Time    :   2026/03/13 17:12:34
@Author  :   loskyertt
@Github  :   https://github.com/loskyertt
@Desc    :   .....
*/

#pragma once

#include "Object.h"
#include "Scene.h"

#include <SDL_events.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_video.h>

#include <functional>
#include <string>
#include <map>

class Game {
 private:
  Scene *currentScene = nullptr;
  bool isRunning = true;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  Background nearStars;  // 近星背景
  Background farStars;   // 远星背景

  TTF_Font *titleFont;  // 标题文字
  TTF_Font *textFont;   // 文本文字

  int finalScore = 0;  // 最终得分

  std::multimap<int, std::string, std::greater<int>> leaderBoard;  // 得分榜

 private:
  int windowWidth = 600;
  int windowHeight = 800;
  bool isFullScreen = false;  // 全屏标志变量
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
  void init();                         // 游戏初始化
  void run();                          // 游戏运行
  void clean();                        // 清理
  void changeScene(Scene *scene);      // 切换场景
  void handleEvent(SDL_Event *event);  // 事件处理
  void update(float time);             // 逻辑更新
  void render();                       // 渲染绘制
  void backgroundUpdate(float time);   // 背景更新
  void renderBackground();             // 渲染背景

  // 渲染文字
  SDL_Point renderTextCentered(const std::string &text, float posY, bool isTitle);      // 渲染标题或普通文本
  void renderTextPos(const std::string &text, int posX, int posY, bool isLeft = true);  // 指定位置渲染文字

  void inserLeaderBoard(const int &score, const std::string &name);  // 插入玩家得分情况

  void saveData();  // 保存排行榜数据
  void loadData();  // 载入排行榜数据

  /* setters：提供设置私有属性的外部接口 */
 public:
  /* 设置最终得分 */
  void setFinalScore(const int &score) { finalScore = score; }

  /* getters：提供获得私有属性的外部接口 */
 public:
  /* 获取窗口 */
  SDL_Window *getWindow() { return window; }

  /* 获取渲染器 */
  SDL_Renderer *getRenderer() { return renderer; }

  /* 获取窗口宽度 */
  int getWindowWidth() { return windowWidth; }

  /* 获取窗口高度 */
  int getWindowHeight() { return windowHeight; }

  /* 获取最终得分 */
  int getFinalScore() { return finalScore; }

  /* 获取得分榜 */
  std::multimap<int, std::string, std::greater<int>> &getLeaderBoard() { return leaderBoard; }  // 返回引用，避免拷贝
};
