#pragma once
#include "enemy.h"
#include <vector>

struct EnemyTiming
{
    int spawnTime;
    EnemyType enemyType;
};

class Levels
{
public:
    Levels();
    std::vector<EnemyTiming> getEnemies(int level);
private:
    std::vector<std::vector<EnemyTiming>> levels;
};


