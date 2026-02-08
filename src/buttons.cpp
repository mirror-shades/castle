#include "buttons.h"
#include <SFML/Graphics.hpp>

sf::Vector2f Buttons::getSize(ButtonType buttonType)
{
    switch (buttonType)
    {
        case ButtonType::MAIN_MENU_PLAY:
            return sf::Vector2f(200.0f, 100.0f);
        case ButtonType::GAME_OVER_MAIN_MENU:
            return sf::Vector2f(250.0f, 80.0f);
        case ButtonType::GACHA_CLAIM:
            return sf::Vector2f(280.0f, 70.0f);
        default:
            return sf::Vector2f(0, 0);
    }
}

sf::Vector2f Buttons::getPosition(sf::RenderWindow& window, ButtonType buttonType)
{
    switch (buttonType)
    {
        case ButtonType::MAIN_MENU_PLAY:
            return sf::Vector2f(window.getSize().x / 2.0f - getSize(buttonType).x / 2.0f, window.getSize().y / 2.0f - getSize(buttonType).y / 2.0f + 100 );
        case ButtonType::GAME_OVER_MAIN_MENU:
            return sf::Vector2f(window.getSize().x / 2.0f - getSize(buttonType).x / 2.0f, window.getSize().y / 2.0f + 80.0f);
        case ButtonType::GACHA_CLAIM:
            return sf::Vector2f(window.getSize().x / 2.0f - getSize(buttonType).x / 2.0f, window.getSize().y / 2.0f + 180.0f);
        default:
            return sf::Vector2f(-1, -1);
    }
}

bool Buttons::isClicked(sf::RenderWindow& window, ButtonType buttonType, sf::Vector2i mousePosition)
{
    sf::Vector2f buttonPos = getPosition(window, buttonType);
    sf::Vector2f buttonSize = getSize(buttonType);
    
    // Check if mouse is within button bounds
    float mouseX = static_cast<float>(mousePosition.x);
    float mouseY = static_cast<float>(mousePosition.y);
    
    return mouseX >= buttonPos.x && 
           mouseX <= buttonPos.x + buttonSize.x &&
           mouseY >= buttonPos.y && 
           mouseY <= buttonPos.y + buttonSize.y;
}
