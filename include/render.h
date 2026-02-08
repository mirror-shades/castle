#pragma once
#include <SFML/Graphics.hpp>
#include "enemy.h"
#include "buttons.h"
#include "state.h"
#include "warrior.h"

class Render
{
public:
    Render();
    
    void drawBackground(sf::RenderWindow& window);
    void drawCastle(sf::RenderWindow& window);
    void drawEnemies(sf::RenderWindow& window, std::vector<Enemy>& enemies);
    void drawMainMenu(sf::RenderWindow& window, Buttons& buttons);
    void drawGameOver(sf::RenderWindow& window, Buttons& buttons, GameData& gameData);
    void drawGame(sf::RenderWindow& window, std::vector<Enemy>& enemies, const std::vector<FireArrowProjectile>& fireArrows, float deltaTime, int level, GameData& gameData);
    void drawLevelComplete(sf::RenderWindow& window, Buttons& buttons, GameData& gameData, const GachaReward& reward, float timer);
    void drawVictory(sf::RenderWindow& window, GameData& gameData);
    
    void setTargetShade(float target);
    void updateShade(float deltaTime);
    void drawShade(sf::RenderWindow& window);
    bool isShadeComplete() const;
    
    void startLevelIntro(int level);
    bool isLevelIntroComplete() const;
    
private:
    void drawEnemy(sf::RenderWindow& window, Enemy& enemy);
    void drawWarriorSlots(sf::RenderWindow& window);
    void drawFireArcherInSlotOne(sf::RenderWindow& window);
    void drawFireArrows(sf::RenderWindow& window, const std::vector<FireArrowProjectile>& fireArrows);
    void drawHealthBar(sf::RenderWindow& window, int currentHealth, int maxHealth);
    void drawXPBar(sf::RenderWindow& window, GameData& gameData);
    void drawLevelIntro(sf::RenderWindow& window, float deltaTime);
    void drawGradient(sf::RenderWindow& window, int startY, int endY, 
                      sf::Color startColor, sf::Color endColor);
    void drawCenteredText(sf::RenderWindow& window, const std::string& str, 
                          unsigned int size, float y, sf::Color color);
    
    sf::Font font;
    
    float currentShade = 0.78f;
    float targetShade = 0.78f;
    static constexpr float shadeSpeed = 2.0f;
    
    int currentLevel = 1;
    float levelIntroTimer = 0.0f;
    bool levelIntroActive = false;
    
    static constexpr float INTRO_FADE_IN_END = 1.0f;
    static constexpr float INTRO_HOLD_END = 2.5f;
    static constexpr float INTRO_FADE_OUT_END = 3.5f;
};
