#include "SceneMain.h"
#include "Game.h"
#include "Object.h"

#include <SDL_image.h>
#include <SDL_keyboard.h>
#include <SDL_log.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <SDL_timer.h>

#include <cstddef>
#include <random>

SceneMain::SceneMain() : game(Game::getInstance()) {}

SceneMain::~SceneMain() {}

void SceneMain::init() {
  // 随机敌人初始化
  std::random_device rd;
  gen = std::mt19937(rd());
  dis = std::uniform_real_distribution<float>(0.0f, 1.0f);
  float r = dis(gen);  // 获取随机数

  // 玩家属性初始化
  player.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
  SDL_QueryTexture(player.texture, NULL, NULL, &player.width,
                   &player.height);  // 根据 texture 设置宽和高
  player.width /= 4;
  player.height /= 4;
  player.position.x =
      static_cast<float>((game.getWindowWidth() - player.width) / 2);
  player.position.y =
      static_cast<float>(game.getWindowHeight() - player.height);

  // 玩家子弹模板初始化
  projectilePlayerTemplate.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/laser-1.png");
  SDL_QueryTexture(
      projectilePlayerTemplate.texture, NULL, NULL,
      &projectilePlayerTemplate.width,
      &projectilePlayerTemplate.height);  // 根据 texture 设置宽和高
  projectilePlayerTemplate.width /= 4;
  projectilePlayerTemplate.height /= 4;

  // 敌机模板初始化
  enemyTempalte.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/insect-2.png");
  SDL_QueryTexture(enemyTempalte.texture, NULL, NULL, &enemyTempalte.width,
                   &enemyTempalte.height);
  enemyTempalte.width /= 4;
  enemyTempalte.height /= 4;
}

void SceneMain::update(float time) {
  keyboardControl(time);
  // SDL_Log("Player position: %f, %f", static_cast<double>(player.position.x), static_cast<double>(player.position.y));
  updatePlayerProjectiles(time);
  spawnEnemy();
  updateEnemies(time);
}

void SceneMain::render() {
  // 渲染玩家的子弹
  // 先渲染子弹，再渲染玩家，可以保证玩家图层在子弹图层之上
  renderPlayerProjectiles();

  // 渲染玩家
  SDL_Rect playerRect = {static_cast<int>(player.position.x),
                         static_cast<int>(player.position.y), player.width,
                         player.height};
  SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);
}

void SceneMain::clean() {
  // 清理容器
  for (auto &projectile : projectilesPlayer) {
    if (projectile != nullptr) {
      delete projectile;  // 删除容器中的指针
    }
  }
  projectilesPlayer.clear();  // 清空容器

  if (player.texture != nullptr) {
    SDL_DestroyTexture(player.texture);
  }
  if (projectilePlayerTemplate.texture != nullptr) {
    SDL_DestroyTexture(projectilePlayerTemplate.texture);
  }
}

void SceneMain::handleEvent(SDL_Event *event) {}

void SceneMain::keyboardControl(float time) {
  auto keyboardState = SDL_GetKeyboardState(NULL);

  // 移动控制逻辑
  if (keyboardState[SDL_SCANCODE_W]) {
    player.position.y -= player.speed * time;
  }
  if (keyboardState[SDL_SCANCODE_S]) {
    player.position.y += player.speed * time;
  }
  if (keyboardState[SDL_SCANCODE_A]) {
    player.position.x -= player.speed * time;
  }
  if (keyboardState[SDL_SCANCODE_D]) {
    player.position.x += player.speed * time;
  }

  // 移动飞船范围限制（别超出游戏窗口）
  if (player.position.x < 0) {
    player.position.x = 0;
  }
  if (player.position.x >
      static_cast<float>((game.getWindowWidth() - player.width))) {
    player.position.x =
        static_cast<float>((game.getWindowWidth() - player.width));
  }
  if (player.position.y < 0) {
    player.position.y = 0;
  }
  if (player.position.y >
      static_cast<float>(game.getWindowHeight() - player.height)) {
    player.position.y =
        static_cast<float>(game.getWindowHeight() - player.height);
  }

  // 控制子弹发射
  if (keyboardState[SDL_SCANCODE_J]) {
    auto currentTime = SDL_GetTicks();
    if (currentTime - player.lastTime > player.coolDown) {
      shootPlayer();
      player.lastTime = currentTime;
    }
  }
}

void SceneMain::shootPlayer() {
  // 发射子弹逻辑实现：发射的时候就在堆内存上创建一个玩家子弹实例
  ProjectilePlayer *projectile = new ProjectilePlayer(
      projectilePlayerTemplate);  // 在堆上创建一个新对象，内容复制自 projectilePlayerTemplate（浅拷贝）
  projectile->position.x = player.position.x +
                           static_cast<float>(player.width) / 2 -
                           static_cast<float>(projectile->width) / 2;
  projectile->position.y = player.position.y;
  projectilesPlayer.push_back(projectile);
}

void SceneMain::updatePlayerProjectiles(float time) {
  int margin = 32;

  for (auto it = projectilesPlayer.begin(); it != projectilesPlayer.end();) {
    auto projectile = *it;
    projectile->position.y -= projectile->speed * time;

    // 检查子弹是否超出屏幕（超出就销毁）
    if (projectile->position.y + static_cast<float>(margin) < 0) {
      delete projectile;
      it = projectilesPlayer.erase(it);
      // SDL_Log("Player projectile removed");
    } else {
      ++it;
    }
  }
}

void SceneMain::renderPlayerProjectiles() {
  for (auto projectile : projectilesPlayer) {
    SDL_Rect projectileRect = {static_cast<int>(projectile->position.x),
                               static_cast<int>(projectile->position.y),
                               projectile->width, projectile->height};
    SDL_RenderCopy(game.getRenderer(), projectile->texture, NULL,
                   &projectileRect);
  }
}

/* 生成敌人 */
void SceneMain::spawnEnemy() {
  // 利用随机数决定是否生成敌机（每帧约 1/60 的概率）
  if (dis(gen) > 1 / 60.0f) {
    return;
  }

  Enemy *enemy = new Enemy(enemyTempalte);
  enemy->position.x =
      dis(gen) * static_cast<float>(game.getWindowWidth() - enemy->width);
  enemy->position.y = static_cast<float>(-enemy->height);
  enemies.push_back(enemy);
}

/* 更新敌机 */
void SceneMain::updateEnemies(float time) {
  for (auto it = enemies.begin(); it != enemies.end();) {
    auto enemy = *it;
    enemy->position.y += enemy->speed * time;
    if (enemy->position.y > static_cast<float>(game.getWindowHeight())) {
      delete enemy;
      it = enemies.erase(it);
      SDL_Log("Enemy removed");
    } else {
      ++it;
    }
  }
}
