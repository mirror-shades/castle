#pragma once
#include <SFML/Graphics.hpp>

enum class ButtonType
{
    MAIN_MENU_PLAY,
    GAME_OVER_MAIN_MENU,
    GACHA_CLAIM,        // Claim reward and continue to next level
};

class Buttons
{
public:
    sf::Vector2f getPosition(sf::RenderWindow& window, ButtonType buttonType);
    sf::Vector2f getSize(ButtonType buttonType);
    bool isClicked(sf::RenderWindow& window, ButtonType buttonType, sf::Vector2i mousePosition);
};
