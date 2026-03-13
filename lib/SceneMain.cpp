/*
@File    :   lib\SceneMain.cpp
@Time    :   2026/03/13 17:13:14
@Author  :   loskyertt
@Github  :   https://github.com/loskyertt
@Desc    :   .....
*/

#include "SceneMain.h"
#include "Object.h"
#include "Game.h"
#include "SceneEnd.h"
#include "SceneTitle.h"

#include <SDL_image.h>
#include <SDL_keyboard.h>
#include <SDL_log.h>
#include <SDL_mixer.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <SDL_stdinc.h>
#include <SDL_surface.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>

#include <cmath>
#include <cstddef>
#include <random>
#include <string>

SceneMain::SceneMain() {}

SceneMain::~SceneMain() {}

void SceneMain::init() {
  // 读取并播放音乐资源
  bgm = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
  if (!bgm) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load music: %s\n", Mix_GetError());
  }
  Mix_PlayMusic(bgm, -1);

  // 读取音效资源
  sounds["player_shoot"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
  sounds["enemy_shoot"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
  sounds["player_explode"] = Mix_LoadWAV("assets/sound/explosion1.wav");
  sounds["enemy_explode"] = Mix_LoadWAV("assets/sound/explosion3.wav");
  sounds["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");
  sounds["get_item"] = Mix_LoadWAV("assets/sound/eff5.wav");

  // 读取 uiHealth
  uiHealth = IMG_LoadTexture(game.getRenderer(), "assets/image/Health UI Black.png");

  // 载入分数字体
  scoreFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);  // 选择的字体大小尽量为缩放的倍数

  // 始化敌机生成的随机数
  std::random_device rd;
  gen = std::mt19937(rd());
  dis = std::uniform_real_distribution<float>(0.0f, 1.0f);
  // float r = dis(gen);  // 获取随机数

  // 玩家属性初始化
  player.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
  SDL_QueryTexture(player.texture, NULL, NULL, &player.width,
                   &player.height);  // 根据 texture 设置宽和高
  player.width /= 4;
  player.height /= 4;
  player.position.x = static_cast<float>((game.getWindowWidth() - player.width)) / 2;
  player.position.y = static_cast<float>(game.getWindowHeight() - player.height);

  // 玩家子弹模板初始化
  projectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-1.png");
  SDL_QueryTexture(
      projectilePlayerTemplate.texture,
      NULL,
      NULL,
      &projectilePlayerTemplate.width,
      &projectilePlayerTemplate.height);  // 根据 texture 设置宽和高
  projectilePlayerTemplate.width /= 4;
  projectilePlayerTemplate.height /= 4;

  // 敌机模板初始化
  enemyTempalte.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-2.png");
  SDL_QueryTexture(enemyTempalte.texture, NULL, NULL, &enemyTempalte.width, &enemyTempalte.height);
  enemyTempalte.width /= 4;
  enemyTempalte.height /= 4;

  // 敌机子弹模板初始化
  projectileEnemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet-1.png");
  SDL_QueryTexture(
      projectileEnemyTemplate.texture, NULL, NULL, &projectileEnemyTemplate.width, &projectileEnemyTemplate.height);
  projectileEnemyTemplate.width /= 4;
  projectileEnemyTemplate.height /= 4;

  // 爆炸特效模板初始化
  explosionTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/effect/explosion.png");
  SDL_QueryTexture(explosionTemplate.texture, NULL, NULL, &explosionTemplate.width, &explosionTemplate.height);
  explosionTemplate.totalFrame = explosionTemplate.width / explosionTemplate.height;
  // explosionTemplate.height /= 4;
  explosionTemplate.width = explosionTemplate.height;  // 宽度和高度一样

  // 掉落物（血包）模板初始化
  itemLifeTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_life.png");
  SDL_QueryTexture(itemLifeTemplate.texture, NULL, NULL, &itemLifeTemplate.width, &itemLifeTemplate.height);
  itemLifeTemplate.width /= 4;
  itemLifeTemplate.height /= 4;
}

void SceneMain::update(float time) {
  keyboardControl(time);
  // SDL_Log("Player position: %f, %f", static_cast<double>(player.position.x), static_cast<double>(player.position.y));
  updatePlayerProjectiles(time);
  updateEnemyProjectiles(time);
  spawnEnemy();
  updateEnemies(time);
  updatePlayer(time);
  updateExplosions(time);
  updateItems(time);

  if (isDead) {
    changeSceneDelayed(time, 3);  // 3s 延迟
  }
}

void SceneMain::render() {
  /*
   * 后渲染的图层在先渲染的图层之上
   */

  // 渲染玩家的子弹
  renderPlayerProjectiles();

  // 渲染敌机的子弹
  renderEnemyProjectiles();

  // 渲染玩家
  renderPlayer();

  // 渲染敌机
  renderEnemies();

  // 渲染物品
  renderItems();

  // 渲染爆炸特效
  renderExplosions();

  // 渲染 ui
  renderUI();
}

void SceneMain::clean() {
  // 清理音效容器
  for (auto sound : sounds) {
    if (sound.second != nullptr) {
      Mix_FreeChunk(sound.second);
    }
  }
  sounds.clear();

  // 清理玩家子弹容器
  for (auto &projectile : projectilesPlayer) {
    if (projectile != nullptr) {
      delete projectile;  // 删除容器中的指针
    }
  }
  projectilesPlayer.clear();  // 清空容器

  // 清理敌机容器
  for (auto &enemy : enemies) {
    if (enemy != nullptr) {
      delete enemy;  // 删除容器中的指针
    }
  }
  enemies.clear();  // 清空容器

  // 清理敌机子弹容器
  for (auto &projectile : projectilesEnemy) {
    if (projectile != nullptr) {
      delete projectile;  // 删除容器中的指针
    }
  }
  projectilesEnemy.clear();  // 清空容器

  // 清理爆炸特效容器
  for (auto &explosion : explosions) {
    if (explosion != nullptr) {
      delete explosion;  // 删除容器中的指针
    }
  }
  explosions.clear();  // 清空容器

  // 清理掉落物容器
  for (auto &item : items) {
    if (item != nullptr) {
      delete item;  // 删除容器中的指针
    }
  }
  items.clear();  // 清空容器

  // 清理模板
  if (player.texture != nullptr) {
    SDL_DestroyTexture(player.texture);
  }
  if (projectilePlayerTemplate.texture != nullptr) {
    SDL_DestroyTexture(projectilePlayerTemplate.texture);
  }
  if (enemyTempalte.texture != nullptr) {
    SDL_DestroyTexture(enemyTempalte.texture);
  }
  if (projectileEnemyTemplate.texture != nullptr) {
    SDL_DestroyTexture(projectileEnemyTemplate.texture);
  }
  if (explosionTemplate.texture != nullptr) {
    SDL_DestroyTexture(explosionTemplate.texture);
  }
  if (itemLifeTemplate.texture != nullptr) {
    SDL_DestroyTexture(itemLifeTemplate.texture);
  }

  // 清理音乐资源
  if (bgm != nullptr) {
    Mix_HaltMusic();     // 先停止播放
    Mix_FreeMusic(bgm);  // 再释放
  }

  // 清理 uiHealth
  if (uiHealth != nullptr) {
    SDL_DestroyTexture(uiHealth);
  }

  // 清理字体
  if (scoreFont != nullptr) {
    TTF_CloseFont(scoreFont);
  }
}

void SceneMain::handleEvent(SDL_Event *event) {
  if (event->type == SDL_KEYDOWN) {
    if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
      SceneTitle *sceneTitle = new SceneTitle();
      game.changeScene(sceneTitle);
    }
  }
}

void SceneMain::keyboardControl(float time) {
  if (isDead) {
    return;
  }

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
  if (player.position.x > static_cast<float>((game.getWindowWidth() - player.width))) {
    player.position.x = static_cast<float>((game.getWindowWidth() - player.width));
  }
  if (player.position.y < 0) {
    player.position.y = 0;
  }
  if (player.position.y > static_cast<float>(game.getWindowHeight() - player.height)) {
    player.position.y = static_cast<float>(game.getWindowHeight() - player.height);
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

/* 玩家状态更新 */
void SceneMain::updatePlayer(float time) {
  if (isDead) {
    return;
  }

  if (player.currentHealth <= 0) {
    // TODO: Game over
    auto currentTime = SDL_GetTicks();
    isDead = true;

    // 玩家爆炸
    Explosion *explosion = new Explosion(explosionTemplate);
    explosion->position.x = player.position.x + static_cast<float>(player.width - explosion->width) / 2;
    explosion->position.y = player.position.y + static_cast<float>(player.height - explosion->height) / 2;
    explosion->startTime = currentTime;
    explosions.push_back(explosion);

    // 玩家爆炸的音效
    Mix_PlayChannel(0, sounds["player_explode"], 0);
    return;
  }
}

/* 渲染玩家 */
void SceneMain::renderPlayer() {
  // 玩家没死的时候才渲染
  if (!isDead) {
    SDL_Rect playerRect = {
        static_cast<int>(player.position.x),
        static_cast<int>(player.position.y),
        player.width,
        player.height,
    };
    SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);  // NULL = 使用整张图
  }
}

/* 玩家发射子弹 */
void SceneMain::shootPlayer() {
  // 发射子弹逻辑实现：发射的时候就在堆内存上创建一个玩家子弹实例
  ProjectilePlayer *projectile = new ProjectilePlayer(
      projectilePlayerTemplate);  // 在堆上创建一个新对象，内容复制自 projectilePlayerTemplate（浅拷贝）
  projectile->position.x =
      player.position.x + static_cast<float>(player.width) / 2 - static_cast<float>(projectile->width) / 2;
  projectile->position.y = player.position.y;
  projectilesPlayer.push_back(projectile);

  /*
   * 参数：
   * channel：-1 表示由系统选择空闲通道，这里手动指定 0 通道
   */
  // 玩家发射子弹的音效
  Mix_PlayChannel(0, sounds["player_shoot"], 0);
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
      bool hit = false;  // 判断是否击中敌机
      for (auto &enemy : enemies) {
        // 敌机矩形
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height,
        };
        // 玩家子弹矩形
        SDL_Rect projectileRect = {
            static_cast<int>(projectile->position.x),
            static_cast<int>(projectile->position.y),
            projectile->width,
            projectile->height,
        };
        if (SDL_HasIntersection(&enemyRect, &projectileRect)) {
          hit = true;  // 检测到碰撞
          enemy->currentHealth -= projectile->damage;
          delete projectile;
          it = projectilesPlayer.erase(it);
          Mix_PlayChannel(-1, sounds["hit"], 0);  // 命中的音效
          break;
        }
      }
      // 没有击中敌机，就检测下一个子弹
      if (!hit) {
        ++it;
      }
    }
  }
}

void SceneMain::renderPlayerProjectiles() {
  for (auto projectile : projectilesPlayer) {
    SDL_Rect projectileRect = {
        static_cast<int>(projectile->position.x),
        static_cast<int>(projectile->position.y),
        projectile->width,
        projectile->height,
    };
    SDL_RenderCopy(game.getRenderer(), projectile->texture, NULL, &projectileRect);
  }
}

/* 生成敌人 */
void SceneMain::spawnEnemy() {
  // 利用随机数决定是否生成敌机（每帧约 1/60 的概率）
  if (dis(gen) > 1 / 60.0f) {
    return;
  }

  Enemy *enemy = new Enemy(enemyTempalte);
  enemy->position.x = dis(gen) * static_cast<float>(game.getWindowWidth() - enemy->width);
  enemy->position.y = static_cast<float>(-enemy->height);
  enemies.push_back(enemy);
}

/* 更新敌机 */
void SceneMain::updateEnemies(float time) {
  auto currentTime = SDL_GetTicks();

  for (auto it = enemies.begin(); it != enemies.end();) {
    auto enemy = *it;
    enemy->position.y += enemy->speed * time;
    if (enemy->position.y > static_cast<float>(game.getWindowHeight())) {
      delete enemy;
      it = enemies.erase(it);
      // SDL_Log("Enemy removed");
    } else {
      if (currentTime - enemy->lastTime > enemy->coolDown && !isDead) {
        shootEnemy(enemy);
        enemy->lastTime = currentTime;
      }
      /*
       * 这里单独增加一个敌机的生命值判断，是为了添加爆炸特效。
       * 把敌机和玩家的碰撞检测也放到这里（复用敌机的循环），
       * 当敌机与玩家碰撞时，敌机直接被破坏，玩家生命值减一
       */
      // 玩家矩形
      SDL_Rect playerRect = {
          static_cast<int>(player.position.x),
          static_cast<int>(player.position.y),
          player.width,
          player.height,
      };
      // 敌机矩形
      SDL_Rect enemyRect = {
          static_cast<int>(enemy->position.x),
          static_cast<int>(enemy->position.y),
          enemy->width,
          enemy->height,
      };
      // 玩家未死亡的前提下进行判断（可能会产生 Bug）
      if (!isDead && SDL_HasIntersection(&playerRect, &enemyRect)) {
        player.currentHealth -= 1;  // 玩家生命值减一
        // SDL_Log("Player current health: %d", player.currentHealth);
        enemy->currentHealth = 0;
      }
      if (enemy->currentHealth <= 0) {
        enemyExplode(enemy);  // 引爆敌机
        it = enemies.erase(it);
        // SDL_Log("Enemy destiyed");
      } else {
        ++it;
      }
    }
  }
}

/* 渲染敌机 */
void SceneMain::renderEnemies() {
  for (auto enemy : enemies) {
    SDL_Rect enemyRect = {
        static_cast<int>(enemy->position.x),
        static_cast<int>(enemy->position.y),
        enemy->width,
        enemy->height,
    };
    SDL_RenderCopy(game.getRenderer(), enemy->texture, NULL, &enemyRect);
  }
}

/* 引爆敌机 */
void SceneMain::enemyExplode(Enemy *enemy) {
  // 创建爆炸
  auto currentTime = SDL_GetTicks();
  Explosion *explosion = new Explosion(explosionTemplate);
  explosion->position.x = enemy->position.x + static_cast<float>(enemy->width - explosion->width) / 2;
  explosion->position.y = enemy->position.y + static_cast<float>(enemy->height - explosion->height) / 2;
  explosion->startTime = currentTime;
  explosions.push_back(explosion);

  // 敌机爆炸的音效
  Mix_PlayChannel(-1, sounds["enemy_explode"], 0);

  // 掉落物品（1/2 的概率）
  if (dis(gen) < 0.5f) {
    dropItem(enemy);
  }

  score += 10;  // 敌机爆炸时，让 score 加 10 分

  delete enemy;
}

/* 敌机射击逻辑 */
void SceneMain::shootEnemy(Enemy *enemy) {
  // 发射子弹逻辑实现：发射的时候就在堆内存上创建一个玩家子弹实例
  ProjectileEnemy *projectile = new ProjectileEnemy(
      projectileEnemyTemplate);  // 在堆上创建一个新对象，内容复制自 projectileEnemyTemplate（浅拷贝）
  projectile->position.x = enemy->position.x + static_cast<float>(enemy->width - projectile->width) / 2;
  projectile->position.y = enemy->position.y + static_cast<float>(enemy->height - projectile->height) / 2;
  projectile->direction = getDirection(enemy);
  projectilesEnemy.push_back(projectile);

  // 敌机发射子弹的音效
  Mix_PlayChannel(-1, sounds["enemy_shoot"], 0);
}

/* 计算敌机子弹方向 */
SDL_FPoint SceneMain::getDirection(Enemy *enemy) {
  // 向量 x = player_mid_x - enemy_mid_x
  float x = (player.position.x + static_cast<float>(player.width) / 2) -
      (enemy->position.x + static_cast<float>(enemy->width) / 2);
  // 向量 y = player_mid_y - enemy_mid_y
  float y = (player.position.y + static_cast<float>(player.height) / 2) -
      (enemy->position.y + static_cast<float>(enemy->height) / 2);
  // 归一化：计算方向余弦
  float length = sqrt(x * x + y * y);
  x /= length;
  y /= length;
  return SDL_FPoint{x, y};
}

/* 更新敌机子弹 */
void SceneMain::updateEnemyProjectiles(float time) {
  int margin = 32;

  for (auto it = projectilesEnemy.begin(); it != projectilesEnemy.end();) {
    auto projectile = *it;
    projectile->position.x += projectile->speed * projectile->direction.x * time;
    projectile->position.y += projectile->speed * projectile->direction.y * time;

    // 检查子弹是否超出屏幕（超出就销毁）
    if (projectile->position.y > static_cast<float>(game.getWindowHeight() + margin) ||  // 超出屏幕正下方
        projectile->position.y < -static_cast<float>(margin) ||                          // 超出屏幕正上方
        projectile->position.x > static_cast<float>(game.getWindowWidth() + margin) ||   // 超出屏幕右方
        projectile->position.x < -static_cast<float>(margin)                             // 超出屏幕左方
    ) {
      delete projectile;
      it = projectilesEnemy.erase(it);
      // SDL_Log("Enemy projectile removed");
    } else {
      // 玩家矩形
      SDL_Rect playerRect = {
          static_cast<int>(player.position.x),
          static_cast<int>(player.position.y),
          player.width,
          player.height,
      };
      // 敌机子弹矩形
      SDL_Rect projectileRect = {
          static_cast<int>(projectile->position.x),
          static_cast<int>(projectile->position.y),
          projectile->width,
          projectile->height,
      };
      // 碰撞检测
      if (SDL_HasIntersection(&playerRect, &projectileRect) && !isDead) {
        player.currentHealth -= projectile->damage;
        delete projectile;
        it = projectilesEnemy.erase(it);
        Mix_PlayChannel(-1, sounds["hit"], 0);  // 命中的音效
      } else {
        ++it;
      }
    }
  }
}

/* 渲染敌机的子弹 */
void SceneMain::renderEnemyProjectiles() {
  for (auto projectile : projectilesEnemy) {
    SDL_Rect projectileRect = {
        static_cast<int>(projectile->position.x),
        static_cast<int>(projectile->position.y),
        projectile->width,
        projectile->height,
    };

    // 角度计算
    double angle = static_cast<double>(atan2(projectile->direction.y, projectile->direction.x)) * 180 * M_PI - 90;
    // 扩展渲染：调整敌机子弹角度
    SDL_RenderCopyEx(game.getRenderer(), projectile->texture, NULL, &projectileRect, angle, NULL, SDL_FLIP_NONE);
  }
}

/* 爆炸特效更新 */
void SceneMain::updateExplosions(float time) {
  auto currentTime = SDL_GetTicks();
  for (auto it = explosions.begin(); it != explosions.end();) {
    auto explosion = *it;
    /*
     * 获取爆炸的当前帧：
     * Δt = (当前时间 - 爆炸开始时间) / 1000 => 单位：s
     * 当前帧 = Δt * FPS
     */
    explosion->currentFrame = (currentTime - explosion->startTime) * explosion->FPS / 1000;
    // 如果当前帧 >= 设定的总帧数，说明爆炸已经展示完了
    if (explosion->currentFrame >= explosion->totalFrame) {
      delete explosion;
      it = explosions.erase(it);
    } else {
      ++it;
    }
  }
}

/* 渲染爆炸特效  */
void SceneMain::renderExplosions() {
  for (auto explosion : explosions) {
    /*
     * 爆炸纹理图集是横向排列的：
     * [帧0][帧1][帧2][帧3][帧4][帧5][帧6][帧7]
     */

    /*
     * src.x = currentFrame * width：选择第几帧（水平方向偏移）
     * src.y = 0：所有帧都在同一行（垂直方向），所以 y=0
     * src.width/height：每一帧的尺寸
     */
    // 从纹理图片的哪个位置开始裁剪（源区域），其坐标是纹理内部的像素坐标
    SDL_Rect src = {
        explosion->currentFrame * explosion->width,
        0,
        explosion->width,
        explosion->height,
    };
    // 裁剪出来的内容画到屏幕的哪个位置（目标区域），其坐标是屏幕坐标
    SDL_Rect dist = {
        static_cast<int>(explosion->position.x),
        static_cast<int>(explosion->position.y),
        explosion->width,
        explosion->height,
    };
    SDL_RenderCopy(game.getRenderer(), explosion->texture, &src, &dist);
  }
}

/* 掉落物品 */
void SceneMain::dropItem(Enemy *enemy) {
  auto item = new Item(itemLifeTemplate);
  item->position.x = enemy->position.x + static_cast<float>(enemy->width - item->width) / 2;
  item->position.y = enemy->position.y + static_cast<float>(enemy->height - item->height) / 2;

  // 设定掉落物起始运动方向
  float angle = dis(gen) * 2 * static_cast<float>(M_PI);
  item->direction.x = cos(angle);
  item->direction.y = sin(angle);

  items.push_back(item);
}

/* 玩家获得物品 */
void SceneMain::playerGetItem(Item *item) {
  score += 5;  // 获得物品时 +5 分

  if (item->type == ItemType::Life && player.currentHealth < player.maxHealth) {
    player.currentHealth += 1;
    // SDL_Log("Player current health: %d", player.currentHealth);
  }

  // 拾取物品的音效
  Mix_PlayChannel(-1, sounds["get_item"], 0);
}

/* 更新物品 */
void SceneMain::updateItems(float time) {
  for (auto it = items.begin(); it != items.end();) {
    auto item = *it;

    // 更新位置
    item->position.x += item->direction.x * item->speed * time;
    item->position.y += item->direction.y * item->speed * time;

    // 处理屏幕边缘反弹，前提：反弹次数 > 0
    if (item->bounceCount > 0) {
      // x 方向反向条件
      if (item->position.x < 0 ||
          (item->position.x + static_cast<float>(item->width) > static_cast<float>(game.getWindowWidth()))) {
        item->direction.x = -item->direction.x;
        --item->bounceCount;
      }
      // y 方向反向条件
      if (item->position.y < 0 ||
          (item->position.y + static_cast<float>(item->width) > static_cast<float>(game.getWindowHeight()))) {
        item->direction.y = -item->direction.y;
        --item->bounceCount;
      }
    }

    // 检查物品是否超出屏幕（超出就销毁）
    if (item->position.y > static_cast<float>(game.getWindowHeight()) ||  // 超出屏幕正下方
        item->position.y < 0 ||                                           // 超出屏幕正上方
        item->position.x > static_cast<float>(game.getWindowWidth()) ||   // 超出屏幕右方
        item->position.x < 0                                              // 超出屏幕左方
    ) {
      delete item;
      it = items.erase(it);
    }
    // 碰撞检测
    else {
      // 掉落物矩形
      SDL_Rect itemRect = {
          static_cast<int>(item->position.x),
          static_cast<int>(item->position.y),
          item->width,
          item->height,
      };
      // 玩家矩形
      SDL_Rect playerRect = {
          static_cast<int>(player.position.x),
          static_cast<int>(player.position.y),
          player.width,
          player.height,
      };
      if (!isDead && SDL_HasIntersection(&itemRect, &playerRect)) {
        playerGetItem(item);  // 玩家获得物品
        delete item;
        it = items.erase(it);
      } else {
        ++it;
      }
    }
  }
}

/* 渲染物品 */
void SceneMain::renderItems() {
  for (auto item : items) {
    SDL_Rect itemRect = {
        static_cast<int>(item->position.x),
        static_cast<int>(item->position.y),
        item->width,
        item->height,
    };

    SDL_RenderCopy(game.getRenderer(), item->texture, NULL, &itemRect);
  }
}

/* 渲染 UI */
void SceneMain::renderUI() {
  /* === 渲染血条 === */
  int x = 10, y = 10;
  int size = 32;    // width = size, height = size
  int offset = 40;  // 每个血量 UI 的偏移量

  // 浅色生命值
  SDL_SetTextureColorMod(uiHealth, 100, 100, 100);  // 比如 r = 当前图片颜色 * (100 / 255)
  for (int i = 0; i < player.maxHealth; ++i) {
    SDL_Rect dist = {
        x + i * offset,
        y,
        size,
        size,
    };
    SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &dist);
  }

  // 深色生命值
  SDL_SetTextureColorMod(uiHealth, 255, 255, 255);  // 比如 r = 当前图片颜色 * (255 / 255)
  for (int i = 0; i < player.currentHealth; ++i) {
    SDL_Rect dist = {
        x + i * offset,
        y,
        size,
        size,
    };
    SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &dist);
  }

  /* === 渲染得分 === */
  auto text = "SCORE:" + std::to_string(score);
  SDL_Color textColor = {255, 255, 255, 255};  // 白色
  SDL_Surface *scoreTextSurface = TTF_RenderUTF8_Solid(scoreFont, text.c_str(), textColor);
  SDL_Texture *scoreTextTexture = SDL_CreateTextureFromSurface(game.getRenderer(), scoreTextSurface);
  SDL_Rect scoreTextRect = {
      game.getWindowWidth() - 10 - scoreTextSurface->w,
      10,
      scoreTextSurface->w,
      scoreTextSurface->h,
  };
  SDL_RenderCopy(game.getRenderer(), scoreTextTexture, NULL, &scoreTextRect);
  SDL_FreeSurface(scoreTextSurface);
  SDL_DestroyTexture(scoreTextTexture);
}

/* 延迟更新结束画面 */
void SceneMain::changeSceneDelayed(float time, float delay) {
  timerEnd += time;
  if (timerEnd > delay) {
    Scene *sceneEnd = new SceneEnd();
    game.setFinalScore(score);  // 在切换退出场景之前先记录分数
    game.changeScene(sceneEnd);
  }
}
