#include "Game.h"
#include "SceneMain.h"

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_log.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <cstddef>

Game::Game() {}

// 单例模式实现
Game &Game::getInstance() {
  static Game instance;
  return instance;
}

Game::~Game() {
  clean();
}

void Game::run() {
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

  // 初始化背景卷轴
  nearStars.texture = IMG_LoadTexture(getRenderer(), "assets/image/Stars-A.png");
  if (!nearStars.texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load nearStars: %s", IMG_GetError());
  }
  SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width, &nearStars.height);
  nearStars.width /= 2;
  nearStars.height /= 2;
  farStars.texture = IMG_LoadTexture(getRenderer(), "assets/image/Stars-B.png");
  if (!farStars.texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load nearStars: %s", IMG_GetError());
  }
  SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width, &farStars.height);
  farStars.speed = 20.0f;
  farStars.width /= 2;
  farStars.height /= 2;

  // 创建当前场景
  currentScene = new SceneMain();
  currentScene->init();
}

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

  // 清理图片资源
  IMG_Quit();

  // 清理音频资源
  Mix_CloseAudio();
  Mix_Quit();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Game::changeScene(Scene *scene) {
  if (currentScene != nullptr) {
    currentScene->clean();
    delete currentScene;
  }
  currentScene = scene;
  currentScene->init();
}

void Game::handleEvent(SDL_Event *event) {
  while (SDL_PollEvent(event)) {
    if (event->type == SDL_QUIT) {
      isRunning = false;
    }

    currentScene->handleEvent(event);
  }
}

void Game::update(float time) {
  backgroundUpdate(time);
  currentScene->update(time);
}

void Game::render() {
  // 清空
  SDL_RenderClear(renderer);

  // 渲染星空背景
  renderBackground();

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
