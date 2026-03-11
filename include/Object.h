#pragma once

#include <SDL_render.h>
#include <SDL_stdinc.h>

/* 玩家 */
struct Player {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0}; // 玩家飞船坐标
  int width = 0;
  int height = 0;
  float speed = 200.0f;
  Uint32 coolDown = 500; // 冷却时间，单位：ms
  Uint32 lastTime = 0;  // 上一次射击时间
};

/* 敌机 */
struct Enemy {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  float speed = 200.0f;
};

/* 子弹 */
struct ProjectilePlayer {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  float speed = 400.0f;
};
