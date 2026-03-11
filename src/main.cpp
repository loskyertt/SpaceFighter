#include <iostream>
#include "Game.h"

int main(int argc, char *argv[]) {
  std::cout << "Hello, Space Fighter!" << std::endl;

  Game game;
  game.init();
  game.run();

  return 0;
}
