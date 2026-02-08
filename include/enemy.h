#pragma once
#include <SFML/Graphics.hpp>

enum class EnemyType
{
    Goblin,
    Orc,
    Skeleton,
    Troll
};

enum class EnemyState
{
    Idle,
    Walking,
    Attacking,
    Dead,
    Grabbed,
    Falling
};

struct Enemy
{
    int hp;
    EnemyType type;
    sf::Vector2f position;
    EnemyState state;
    float animationTime = 0.0f;  // Accumulates time for animations
    float speed = 100.0f;        // Movement speed in pixels per second
    float baseY = 0.0f;          // Ground Y position (where enemy spawned/walks)
    sf::Vector2f velocity = {0.0f, 0.0f};  // Current velocity for physics
    float deathTime = 0.0f;      // Time since death, drives death animation
    float scale = 1.0f;          // Visual/hitbox scale based on enemy type
    float attackTimer = 0.0f;    // Accumulator for attack damage ticks
    float attackInterval = 1.0f; // Seconds between damage ticks
    int attackDamage = 5;        // Damage dealt to castle per tick
    int xpReward = 10;           // XP earned when killed
};
