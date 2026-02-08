#include "core.h"
#include <cstdlib>

Enemy spawnEnemy(EnemyType type, sf::Vector2f position)
{
    Enemy enemy;
    enemy.type = type;
    enemy.position = position;
    enemy.baseY = position.y;  // Store ground level for fall damage calculation
    enemy.state = EnemyState::Walking;
    enemy.animationTime = 0.0f;
    enemy.velocity = {0.0f, 0.0f};

    // Randomize stats based on enemy type
    switch (type)
    {
        case EnemyType::Goblin:
            enemy.hp = 30 + rand() % 6;      // 30-35 hp
            enemy.speed = 50.0f + static_cast<float>(rand() % 41);  // 50-90 px/s
            enemy.scale = 0.8f;
            enemy.attackDamage = 3;
            enemy.attackInterval = 1.0f;
            enemy.xpReward = 5;
            break;

        case EnemyType::Orc:
            enemy.hp = 60 + rand() % 11;     // 60-70 hp
            enemy.speed = 80.0f + static_cast<float>(rand() % 31);  // 80-110 px/s
            enemy.scale = 1.2f;
            enemy.attackDamage = 5;
            enemy.attackInterval = 1.2f;
            enemy.xpReward = 15;
            break;

        case EnemyType::Skeleton:
            enemy.hp = 20 + rand() % 6;      // 20-25 hp
            enemy.speed = 140.0f + static_cast<float>(rand() % 41);  // 140-180 px/s
            enemy.scale = 1.0f;
            enemy.attackDamage = 2;
            enemy.attackInterval = 0.8f;
            enemy.xpReward = 8;
            break;

        case EnemyType::Troll:
            enemy.hp = 100 + rand() % 21;    // 100-120 hp
            enemy.speed = 60.0f + static_cast<float>(rand() % 31);  // 60-90 px/s
            enemy.scale = 1.8f;
            enemy.attackDamage = 8;
            enemy.attackInterval = 1.5f;
            enemy.xpReward = 30;
            break;
    }

    return enemy;
}

void checkAndSpawnEnemies(float elapsedTime,
                          const std::vector<EnemyTiming>& timings,
                          size_t& nextSpawnIndex,
                          std::vector<Enemy>& enemies)
{
    // Spawn all enemies whose time has come
    while (nextSpawnIndex < timings.size())
    {
        const EnemyTiming& timing = timings[nextSpawnIndex];
        
        // Check if it's time to spawn this enemy
        if (elapsedTime >= static_cast<float>(timing.spawnTime))
        {
            // Randomize Y position on the ground area (500-650), X starts off-screen left
            float spawnY = 500.0f + static_cast<float>(rand() % 151);
            Enemy enemy = spawnEnemy(timing.enemyType, {-50.0f, spawnY});
            enemies.push_back(enemy);
            nextSpawnIndex++;
        }
        else
        {
            // Timings are in order, so if this one isn't ready, none after are
            break;
        }
    }
}

