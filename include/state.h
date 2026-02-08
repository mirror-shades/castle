#pragma once
#include <string>

enum class GameState
{
    MainMenu,
    Game,
    Pause,
    LevelComplete,
    GameOver,
    Victory
};

enum class RewardType
{
    HealthRestore,
    MaxHealthUp,
    DamageResistance,
    XPMultiplier,
    CastleFortify
};

struct GachaReward
{
    RewardType type;
    std::string name;
    std::string description;
    int value;
    int rarity;
};

struct GameData
{
    int level = 1;
    int currentHealth = 100;
    int maxHealth = 100;
    int xp = 0;
    int totalXP = 0;
    float damageReduction = 0.0f;
    float xpMultiplier = 1.0f;
};
