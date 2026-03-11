#include "Game.h"
#include "SceneMain.h"

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_log.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <SDL_image.h>

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
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    isRunning = false;
  }

  // 创建窗口
  window = SDL_CreateWindow("飞船大战", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight,
                            SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Window could not be created! SDL_Error: %s\n",
                 SDL_GetError());
    isRunning = false;
  }

  // 创建渲染器
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "Renderer could not be created! SDL_Error: %s\n",
                 SDL_GetError());
    isRunning = false;
  }

  // 初始化 SDL_image
  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    SDL_Log("SDL_image could not initialize! SDL_image Error: %s\n",
            IMG_GetError());
    isRunning = false;
  }

  // 创建当前场景
  currentScene = new SceneMain();
  currentScene->init();
}

void Game::clean() {
  if (currentScene != nullptr) {
    currentScene->clean();
    delete currentScene;
  }

  // 清理图片资源
  IMG_Quit();

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
  currentScene->update(deltaTime);
}

void Game::render() {
  // 清空
  SDL_RenderClear(renderer);

  currentScene->render();

  // 显示更新
  SDL_RenderPresent(renderer);
}
