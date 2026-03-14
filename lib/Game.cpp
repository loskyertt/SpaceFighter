/*
@File    :   lib\Game.cpp
@Time    :   2026/03/13 17:12:46
@Author  :   loskyertt
@Github  :   https://github.com/loskyertt
@Desc    :   .....
*/

#include "Game.h"
#include "SceneTitle.h"

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_log.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <fstream>
#include <string>

Game::Game() {}

// 单例模式实现
Game &Game::getInstance() {
  static Game instance;
  return instance;
}

Game::~Game() {
  clean();
}

/*
 * 游戏初始化：
 * - SDL 初始化
 * - 创建窗口
 * - 创建渲染器
 * - 初始化 SDL_image
 * - 初始化 SDL_mixer
 * - 初始化 SDL_ttf
 * - 初始化背景卷轴
 * - 创建当前场景
 */
void Game::init() {
  frameTime = static_cast<Uint32>(1000 / FPS);

  // SDL 初始化
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    isRunning = false;
  }

  // 创建窗口
  window = SDL_CreateWindow(
      "飞船大战", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
    isRunning = false;
  }

  // 创建渲染器
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    isRunning = false;
  }

  // 设计逻辑分辨率
  SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);

  // 初始化 SDL_image
  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    isRunning = false;
  }

  // 初始化 SDL_mixer
  if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    isRunning = false;
  }
  // 打开音频设备
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    isRunning = false;
  }
  Mix_AllocateChannels(32);             // 设置可支持的音效数量
  Mix_VolumeMusic(MIX_MAX_VOLUME / 4);  // 设置背景音乐音量
  Mix_Volume(-1, MIX_MAX_VOLUME / 8);   // 设置音效音量

  // 初始化 SDL_ttf
  if (TTF_Init() == -1) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
  }

  // 初始化背景卷轴
  nearStars.texture = IMG_LoadTexture(getRenderer(), "assets/image/Stars-A.png");
  SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width, &nearStars.height);
  if (!nearStars.texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load nearStars background: %s", IMG_GetError());
  }
  nearStars.width /= 2;
  nearStars.height /= 2;

  farStars.texture = IMG_LoadTexture(getRenderer(), "assets/image/Stars-B.png");
  SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width, &farStars.height);
  if (!farStars.texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load farStars background: %s", IMG_GetError());
  }
  farStars.speed = 20.0f;
  farStars.width /= 2;
  farStars.height /= 2;

  // 载入字体
  titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
  textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);
  if (!titleFont || !textFont) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load fonts: %s", TTF_GetError());
  }

  // 载入得分
  loadData();

  // 创建当前场景
  // currentScene = new SceneMain();   // 主场景
  currentScene = new SceneTitle();  // 标题场景
  currentScene->init();             // 初始化当前场景
}

/* 游戏运行 */
void Game::run() {
  /*
   * 渲染循环：
   * 外层 while(isRunning) 循环：主游戏循环，持续运行直到程序退出。
   * 内层 handleEvent(&event) 循环：用于处理所有待处理的SDL事件（如键盘、鼠标、窗口关闭等）。
   */
  while (isRunning) {
    auto frameStart = SDL_GetTicks();  // 记录开始时刻

    SDL_Event event;
    handleEvent(&event);
    update(deltaTime);
    render();

    auto frameEnd = SDL_GetTicks();  // 记录结束时刻
    auto diff = frameEnd - frameStart;
    if (diff < frameTime) {
      SDL_Delay(frameTime - diff);
      deltaTime = static_cast<float>(frameTime) / 1000.0f;  // 将 ms 转换为 s
    } else {
      deltaTime = static_cast<float>(diff) / 1000.0f;
    }
  }
}

/* 清理 */
void Game::clean() {
  if (currentScene != nullptr) {
    currentScene->clean();
    delete currentScene;
  }

  if (nearStars.texture != nullptr) {
    SDL_DestroyTexture(nearStars.texture);
  }
  if (farStars.texture != nullptr) {
    SDL_DestroyTexture(farStars.texture);
  }

  if (titleFont != nullptr) {
    TTF_CloseFont(titleFont);
  }
  if (textFont != nullptr) {
    TTF_CloseFont(textFont);
  }

  // 清理图片资源
  IMG_Quit();

  // 清理音频资源
  Mix_CloseAudio();
  Mix_Quit();

  // 清理字体资源
  TTF_Quit();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

/* 切换场景 */
void Game::changeScene(Scene *scene) {
  if (currentScene != nullptr) {
    currentScene->clean();
    delete currentScene;
  }
  currentScene = scene;
  currentScene->init();
}

/* 事件处理 */
void Game::handleEvent(SDL_Event *event) {
  while (SDL_PollEvent(event)) {
    if (event->type == SDL_QUIT) {
      saveData();
      SDL_Log("Save data");
      isRunning = false;
    }

    if (event->key.keysym.scancode == SDL_SCANCODE_F4) {
      isFullScreen = !isFullScreen;
      if (isFullScreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
      } else {
        SDL_SetWindowFullscreen(window, 0);
      }
    }

    currentScene->handleEvent(event);
  }
}

/* 逻辑更新 */
void Game::update(float time) {
  backgroundUpdate(time);
  currentScene->update(time);
}

/* 渲染绘制 */
void Game::render() {
  // 清空
  SDL_RenderClear(renderer);

  // 渲染星空背景
  renderBackground();

  // 渲染当前场景
  currentScene->render();

  // 显示更新
  SDL_RenderPresent(renderer);
}

/* 背景更新 */
void Game::backgroundUpdate(float time) {
  // 近星
  nearStars.y_offset += time * nearStars.speed;
  if (nearStars.y_offset >= 0) {
    nearStars.y_offset -= static_cast<float>(nearStars.height);
  }

  // 远星
  farStars.y_offset += time * farStars.speed;
  if (farStars.y_offset >= 0) {
    farStars.y_offset -= static_cast<float>(farStars.height);
  }
}

/* 渲染背景 */
void Game::renderBackground() {
  // 渲染远星
  for (int posY = static_cast<int>(farStars.y_offset); posY < getWindowHeight(); posY += farStars.height) {
    for (int posX = 0; posX < getWindowWidth(); posX += farStars.width) {
      SDL_Rect distRect = {
          posX,
          posY,
          farStars.width,
          farStars.height,
      };
      SDL_RenderCopy(getRenderer(), farStars.texture, NULL, &distRect);
    }
  }

  // 渲染近星
  for (int posY = static_cast<int>(nearStars.y_offset); posY < getWindowHeight(); posY += nearStars.height) {
    for (int posX = 0; posX < getWindowWidth(); posX += nearStars.width) {
      SDL_Rect distRect = {
          posX,
          posY,
          nearStars.width,
          nearStars.height,
      };
      SDL_RenderCopy(getRenderer(), nearStars.texture, NULL, &distRect);
    }
  }
}

/* 渲染标题或普通文本
 * - text: 要渲染的文本
 * - posY: 相对位置，即比例系数，范围 (0, 1)
 * - isTitle: 是否是标题
*/
SDL_Point Game::renderTextCentered(const std::string &text, float posY, bool isTitle) {
  SDL_Color color = {255, 255, 255, 255};  // 白色

  SDL_Surface *surface;
  if (isTitle) {
    surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
  } else {
    surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(getRenderer(), surface);
  int y = static_cast<int>(static_cast<float>((getWindowHeight() - surface->h)) * posY);
  SDL_Rect fontRect = {
      (getWindowWidth() - surface->w) / 2,  // 横坐标为中心
      y,
      surface->w,
      surface->h,
  };
  SDL_RenderCopy(getRenderer(), texture, NULL, &fontRect);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);

  return {fontRect.x + fontRect.w, y};
}

/* 指定位置渲染文字 */
void Game::renderTextPos(const std::string &text, int posX, int posY, bool isLeft) {
  SDL_Color color = {255, 255, 255, 255};  // 白色
  SDL_Surface *surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(getRenderer(), surface);
  SDL_Rect fontRect;
  if (isLeft) {
    fontRect = {
        posX,  // 横坐标为中心
        posY,
        surface->w,
        surface->h,
    };
  } else {
    fontRect = {
        getWindowWidth() - posX - surface->w,  // 横坐标为中心
        posY,
        surface->w,
        surface->h,
    };
  }
  SDL_RenderCopy(getRenderer(), texture, NULL, &fontRect);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

/* 插入玩家得分情况 */
void Game::inserLeaderBoard(const int &score, const std::string &name) {
  leaderBoard.insert({score, name});
  if (leaderBoard.size() > 8) {
    leaderBoard.erase(--leaderBoard.end());
  }
}

/* 保存排行榜数据 */
void Game::saveData() {
  std::ofstream file("data/save.dat");
  if (!file.is_open()) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open file!");
    return;
  }
  for (const auto &item : leaderBoard) {
    file << item.first << " " << item.second << std::endl;
  }
}

/* 载入排行榜数据 */
void Game::loadData() {
  std::ifstream file("data/save.dat");
  if (!file.is_open()) {
    SDL_Log("Failed to open file!");
    return;
  }

  leaderBoard.clear();
  int score;
  std::string name;
  while (file >> score >> name) {
    inserLeaderBoard(score, name);
  }
}
