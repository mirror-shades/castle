#include "levels.h"

Levels::Levels()
{
    // Level 1 - Goblin scouts
    levels.push_back({
        {5, EnemyType::Goblin},
        {10, EnemyType::Goblin},
        {14, EnemyType::Goblin},
        {17, EnemyType::Goblin},
        {20, EnemyType::Goblin},
        {22, EnemyType::Goblin},
        {22, EnemyType::Goblin},
        {26, EnemyType::Goblin},
        {28, EnemyType::Goblin},
        {31, EnemyType::Goblin},
    });

    // Level 2 - Skeleton rush
    levels.push_back({
        {3, EnemyType::Skeleton},
        {5, EnemyType::Skeleton},
        {7, EnemyType::Goblin},
        {9, EnemyType::Skeleton},
        {11, EnemyType::Skeleton},
        {13, EnemyType::Goblin},
        {14, EnemyType::Skeleton},
        {16, EnemyType::Skeleton},
        {18, EnemyType::Skeleton},
        {20, EnemyType::Goblin},
        {21, EnemyType::Skeleton},
        {23, EnemyType::Skeleton},
    });

    // Level 3 - Orcs arrive
    levels.push_back({
        {4, EnemyType::Goblin},
        {7, EnemyType::Goblin},
        {10, EnemyType::Orc},
        {13, EnemyType::Goblin},
        {15, EnemyType::Orc},
        {17, EnemyType::Skeleton},
        {19, EnemyType::Skeleton},
        {21, EnemyType::Orc},
        {24, EnemyType::Goblin},
        {26, EnemyType::Orc},
        {28, EnemyType::Goblin},
    });

    // Level 4 - Mixed assault
    levels.push_back({
        {3, EnemyType::Skeleton},
        {5, EnemyType::Skeleton},
        {7, EnemyType::Orc},
        {9, EnemyType::Goblin},
        {10, EnemyType::Goblin},
        {12, EnemyType::Orc},
        {14, EnemyType::Skeleton},
        {15, EnemyType::Skeleton},
        {17, EnemyType::Orc},
        {19, EnemyType::Goblin},
        {20, EnemyType::Goblin},
        {22, EnemyType::Orc},
        {24, EnemyType::Skeleton},
        {26, EnemyType::Orc},
    });

    // Level 5 - The troll appears
    levels.push_back({
        {3, EnemyType::Goblin},
        {5, EnemyType::Goblin},
        {8, EnemyType::Orc},
        {11, EnemyType::Skeleton},
        {13, EnemyType::Skeleton},
        {15, EnemyType::Troll},
        {18, EnemyType::Goblin},
        {20, EnemyType::Orc},
        {22, EnemyType::Skeleton},
        {24, EnemyType::Skeleton},
        {26, EnemyType::Orc},
        {28, EnemyType::Goblin},
    });

    // Level 6 - Double trouble
    levels.push_back({
        {2, EnemyType::Orc},
        {4, EnemyType::Skeleton},
        {5, EnemyType::Skeleton},
        {7, EnemyType::Troll},
        {10, EnemyType::Goblin},
        {11, EnemyType::Goblin},
        {13, EnemyType::Orc},
        {15, EnemyType::Orc},
        {17, EnemyType::Skeleton},
        {18, EnemyType::Skeleton},
        {20, EnemyType::Troll},
        {23, EnemyType::Goblin},
        {24, EnemyType::Orc},
        {26, EnemyType::Skeleton},
        {28, EnemyType::Orc},
    });

    // Level 7 - Troll army
    levels.push_back({
        {3, EnemyType::Troll},
        {6, EnemyType::Orc},
        {8, EnemyType::Orc},
        {10, EnemyType::Troll},
        {13, EnemyType::Skeleton},
        {14, EnemyType::Skeleton},
        {15, EnemyType::Skeleton},
        {17, EnemyType::Troll},
        {20, EnemyType::Orc},
        {22, EnemyType::Orc},
        {24, EnemyType::Troll},
        {26, EnemyType::Goblin},
        {27, EnemyType::Goblin},
        {28, EnemyType::Goblin},
        {30, EnemyType::Troll},
    });
}

std::vector<EnemyTiming> Levels::getEnemies(int level)
{
    // Levels are 1-indexed, vector is 0-indexed
    if (level >= 1 && level <= static_cast<int>(levels.size()))
    {
        return levels[level - 1];
    }
    return {};  // Empty vector if level doesn't exist
}
