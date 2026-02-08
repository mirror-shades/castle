#pragma once
#include "enemy.h"
#include "levels.h"
#include <vector>



// Spawns a new enemy of the given type at the specified position
// Stats (hp, speed) are randomized within type-specific ranges
Enemy spawnEnemy(EnemyType type, sf::Vector2f position);

// Checks level timings and spawns any enemies that are due
// elapsedTime: seconds since level started
// timings: the enemy spawn schedule for the current level
// nextSpawnIndex: tracks which enemy to spawn next (updated by function)
// enemies: vector to add spawned enemies to
// Spawns at x=-50 (off-screen left), y randomized between 500-650 (ground area)
void checkAndSpawnEnemies(float elapsedTime, 
                          const std::vector<EnemyTiming>& timings,
                          size_t& nextSpawnIndex,
                          std::vector<Enemy>& enemies);

