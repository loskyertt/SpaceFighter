/*
@File    :   test\test01.cpp
@Time    :   2026/03/13 17:13:31
@Author  :   loskyertt
@Github  :   https://github.com/loskyertt
@Desc    :   .....
*/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

int main(int argc, char *argv[]) {
  // SDL 初始化
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }
  // 创建窗口
  SDL_Window *window = SDL_CreateWindow("Space Fighter", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
  // 创建渲染器
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // SDL_image 初始化
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != (IMG_INIT_PNG | IMG_INIT_JPG)) {
    SDL_Log("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    return 1;
  }
  // 加载图片
  SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/image/bg.png");

  // SDL_mixer 初始化
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    return 1;
  }
  // 加载音乐
  Mix_Music *music = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
  // 播放音乐
  Mix_PlayMusic(music, -1);  // 循环播放

  // SDL_ttf 初始化
  if (TTF_Init() == -1) {
    SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    return 1;
  }
  // 加载字体
  TTF_Font *font = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);
  // 创建纹理
  SDL_Color textColor = {255, 255, 255, 255};  // 白色
  SDL_Surface *surface = TTF_RenderUTF8_Solid(font, "你好呀", textColor);
  SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, surface);

  /*
   * 渲染循环：
   * 外层 while(true) 循环：主游戏循环，持续运行直到程序退出。
   * 内层 while (SDL_PollEvent(&event)) 循环：用于处理所有待处理的SDL事件（如键盘、鼠标、窗口关闭等）。
   */
  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        return 0;  // 退出程序
      }
    }

    // 清屏
    SDL_RenderClear(renderer);

    // 画一个长方形
    SDL_Rect rect = {100, 100, 200, 200};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // 设置绘制颜色为红色，用于绘制后面的 SDL_RenderFillRect 填充矩形
    SDL_RenderFillRect(renderer, &rect);

    // 画图片
    SDL_Rect dstRect = {300, 100, 200, 200};            // 图片显示位置和大小
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);  // NULL = 使用整张图

    // 画文本
    SDL_Rect textRect = {100, 350, surface->w, surface->h};  // 文本显示位置和大小
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // 窗口底色

    // 更新屏幕
    SDL_RenderPresent(renderer);
  }

  // 清理图片资源
  SDL_DestroyTexture(texture);
  IMG_Quit();

  // 清理音乐资源
  Mix_FreeMusic(music);
  Mix_CloseAudio();
  Mix_Quit();

  // 清理字体资源
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(textTexture);
  TTF_CloseFont(font);
  TTF_Quit();

  // 释放资源并退出
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
