#include "Game.h"

int main(int argc, char *argv[]) {
  // std::cout << "Hello, Space Fighter!" << std::endl;

  Game &game = Game::getInstance();
  game.init();  // 游戏初始化
  game.run();   // 游戏运行

  return 0;
}
