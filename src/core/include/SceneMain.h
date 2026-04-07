/*
@File    :   include\SceneMain.h
@Time    :   2026/03/13 17:13:02
@Author  :   loskyertt
@Github  :   https://github.com/loskyertt
@Desc    :   .....
*/

#pragma once

#include "Scene.h"
#include "Object.h"

#include <SDL_mixer.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>

#include <list>
#include <random>
#include <string>
#include <unordered_map>

#pragma once

class SceneMain : public Scene {
 private:
  bool isDead = false;              // 玩家是否死亡
  Mix_Music *bgm;                   // 背景音乐
  SDL_Texture *uiHealth = nullptr;  // 玩家生命值图标
  TTF_Font *scoreFont;              // 分数字体
  int score = 0;                    // 得分信息
  float timerEnd = 0.0f;            // 延迟播放结束画面

  /* 模板在栈上创建，可以先把材质文件加载进模板（内存），毕竟 I/O 很费时 */
 private:
  Player player;                              // 玩家就一个，自然不需要设为模板
  ProjectilePlayer projectilePlayerTemplate;  // 模板：用户子弹
  Enemy enemyTempalte;                        // 模板：敌机
  ProjectileEnemy projectileEnemyTemplate;    // 模板：敌机子弹
  Explosion explosionTemplate;                // 模板：爆炸特效
  Item itemLifeTemplate;                      // 掉落物模板：血包

  // 容器
  std::list<ProjectilePlayer *> projectilesPlayer;      // 容器：玩家子弹
  std::list<Enemy *> enemies;                           // 容器：敌机
  std::list<ProjectileEnemy *> projectilesEnemy;        // 容器：敌机子弹
  std::list<Explosion *> explosions;                    // 容器：爆炸特效
  std::list<Item *> items;                              // 容器：掉落物
  std::unordered_map<std::string, Mix_Chunk *> sounds;  // 容器：音效

 private:
  std::mt19937 gen;                           // 随机数生成器
  std::uniform_real_distribution<float> dis;  // 随机数分布器（均匀分布）

 public:
  SceneMain();
  ~SceneMain() override;

  /* 接口实现 */
 public:
  void init() override;
  void update(float time) override;
  void render() override;
  void clean() override;
  void handleEvent(SDL_Event *event) override;

  /* 游戏场景 */
 private:
  void keyboardControl(float time);  // 键盘逻辑

  void updatePlayer(float time);  // 玩家状态更新
  void renderPlayer();            // 渲染玩家

  void shootPlayer();                        // 玩家射击逻辑
  void updatePlayerProjectiles(float time);  // 更新玩家的子弹
  void renderPlayerProjectiles();            // 渲染玩家的子弹

  void spawnEnemy();                // 生成敌机
  void updateEnemies(float time);   // 更新敌机
  void renderEnemies();             // 渲染敌机
  void enemyExplode(Enemy *enemy);  // 引爆敌机

  void shootEnemy(Enemy *enemy);            // 敌机射击逻辑
  SDL_FPoint getDirection(Enemy *enemy);    // 计算敌机子弹方向
  void updateEnemyProjectiles(float time);  // 更新敌机子弹
  void renderEnemyProjectiles();            // 渲染敌机的子弹

  void updateExplosions(float time);  // 爆炸特效更新
  void renderExplosions();            // 渲染爆炸特效

  void dropItem(Enemy *enemy);     // 掉落物品
  void playerGetItem(Item *item);  // 玩家获得物品
  void updateItems(float time);    // 更新物品
  void renderItems();              // 渲染物品

  void renderUI();  // 渲染 UI

  void changeSceneDelayed(float time, float delay);  // 延迟更新结束画面
};
