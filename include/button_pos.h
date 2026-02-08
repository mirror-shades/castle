#pragma once
#include <SFML/Graphics.hpp>

enum class ButtonType
{
    MAIN_MENU_PLAY,
};

class ButtonPos
{
public:
    sf::Vector2f getPosition(sf::RenderWindow& window, ButtonType buttonType);
    bool isClicked(sf::RenderWindow& window, ButtonType buttonType, sf::Vector2i mousePosition);
};