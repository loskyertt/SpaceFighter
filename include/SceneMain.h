#pragma once

#include "Game.h"
#include "Scene.h"
#include "Object.h"

#include <list>
#include <random>

#pragma once

class SceneMain : public Scene {
 private:
  Game &game;
  Player player;

 private:
  // 在栈上创建一个用户子弹的（被拷贝）模板
  ProjectilePlayer
      projectilePlayerTemplate;  // 可以先把材质文件加载进模板（内存），毕竟 I/O 很费时

  std::list<ProjectilePlayer *> projectilesPlayer;

 private:
  // 在栈上创建一个敌机的（被拷贝）模板
  Enemy enemyTempalte;
  std::list<Enemy *> enemies;

 private:
  std::mt19937 gen;                           // 随机数生成器
  std::uniform_real_distribution<float> dis;  // 随机数分布器（均匀分布）

 public:
  SceneMain();
  ~SceneMain() override;
  void init() override;
  void update(float time) override;
  void render() override;
  void clean() override;
  void handleEvent(SDL_Event *event) override;

 public:
  void keyboardControl(float time);  // 键盘逻辑

  void shootPlayer();                        // 玩家射击逻辑
  void updatePlayerProjectiles(float time);  // 更新玩家子弹
  void renderPlayerProjectiles();            // 渲染玩家子弹

  void spawnEnemy();             // 生成敌机
  void updateEnemies(float time);  // 更新敌机
};
