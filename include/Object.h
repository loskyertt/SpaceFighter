#pragma once

#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>

enum class ItemType { Life, Shield, Time };

/* 玩家 */
struct Player {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};  // 玩家飞船坐标
  int width = 0;
  int height = 0;
  float speed = 300.0f;
  int currentHealth = 3;  // 血量
  int maxHealth = 5;      // 血量上限
  Uint32 coolDown = 300;  // 冷却时间，单位：ms
  Uint32 lastTime = 0;    // 上一次射击时间
};

/* 敌机 */
struct Enemy {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  float speed = 150.0f;
  int currentHealth = 2;   // 血量
  Uint32 coolDown = 2000;  // 冷却时间，单位：ms
  Uint32 lastTime = 0;     // 上一次射击时间
};

/* 玩家的子弹 */
struct ProjectilePlayer {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  float speed = 600.0f;
  int damage = 1;
};

/* 敌机的子弹 */
struct ProjectileEnemy {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  SDL_FPoint direction = {9, 0};
  int width = 0;
  int height = 0;
  float speed = 400.0f;
  int damage = 1;
};

/* 爆炸动画 */
struct Explosion {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  int currentFrame = 0;
  int totalFrame = 0;
  Uint32 startTime = 0;
  Uint32 FPS = 10;
};

/* 凋落物 */
struct Item {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  SDL_FPoint direction = {0, 0};
  int width = 0;
  int height = 0;
  float speed = 200.0f;
  int bounceCount = 3;  // 反弹次数
  ItemType type = ItemType::Life;
};

/* 背景 */
struct Background {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  float y_offset = 0;
  int width = 0;
  int height = 0;
  float speed = 30.0f;
};
