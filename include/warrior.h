#pragma once
#include <SFML/Graphics.hpp>

struct FireArrowProjectile
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    int directDamage = 8;
    float burnDps = 3.0f;
    float burnDuration = 3.0f;
};
