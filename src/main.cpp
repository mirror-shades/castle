
#include <SFML/Graphics.hpp>
#include "render.h"
#include "enemy.h"
#include "state.h"
#include "buttons.h"
#include "core.h"
#include "levels.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

// Generate a random gacha reward based on current level
GachaReward rollGachaReward(int level)
{
    // Higher levels have better odds for rarer rewards
    int roll = rand() % 100;
    int rarity;
    if (roll < std::max(10, 40 - level * 5))       rarity = 3;  // Epic
    else if (roll < std::max(30, 70 - level * 3))   rarity = 2;  // Rare
    else                                              rarity = 1;  // Common

    // Pick a reward type
    int typeRoll = rand() % 5;
    GachaReward reward;
    reward.rarity = rarity;

    switch (typeRoll) {
        case 0:
            reward.type = RewardType::HealthRestore;
            reward.value = 15 + rarity * 10;  // 25/35/45
            reward.name = "Healing Potion";
            reward.description = "Restore " + std::to_string(reward.value) + " castle HP";
            break;
        case 1:
            reward.type = RewardType::MaxHealthUp;
            reward.value = 10 + rarity * 10;  // 20/30/40
            reward.name = "Stone Fortification";
            reward.description = "Max HP +" + std::to_string(reward.value);
            break;
        case 2:
            reward.type = RewardType::DamageResistance;
            reward.value = 3 + rarity * 2;  // 5%/7%/9%
            reward.name = "Iron Plating";
            reward.description = "Damage resistance +" + std::to_string(reward.value) + "%";
            break;
        case 3:
            reward.type = RewardType::XPMultiplier;
            reward.value = 5 + rarity * 5;  // 10%/15%/20%
            reward.name = "Battle Wisdom";
            reward.description = "XP gain +" + std::to_string(reward.value) + "%";
            break;
        case 4:
            reward.type = RewardType::CastleFortify;
            reward.value = 10 + rarity * 8;  // 18/26/34
            reward.name = "Master Builder";
            reward.description = "Restore " + std::to_string(reward.value) + " HP & Max HP +" + std::to_string(reward.value / 2);
            break;
    }

    return reward;
}

// Apply gacha reward effects to game data
void applyReward(GameData& gameData, const GachaReward& reward)
{
    switch (reward.type) {
        case RewardType::HealthRestore:
            gameData.currentHealth = std::min(gameData.maxHealth, gameData.currentHealth + reward.value);
            break;
        case RewardType::MaxHealthUp:
            gameData.maxHealth += reward.value;
            gameData.currentHealth += reward.value / 2;  // Partial heal when max goes up
            break;
        case RewardType::DamageResistance:
            gameData.damageReduction = std::min(0.75f, gameData.damageReduction + reward.value / 100.0f);
            break;
        case RewardType::XPMultiplier:
            gameData.xpMultiplier += reward.value / 100.0f;
            break;
        case RewardType::CastleFortify:
            gameData.maxHealth += reward.value / 2;
            gameData.currentHealth = std::min(gameData.maxHealth, gameData.currentHealth + reward.value);
            break;
    }
}

int main()
{
    // Seed random number generator
    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(1200, 720), "Castle Defense");
    Render render;
    GameState gameState = GameState::MainMenu;
    Buttons buttons;
    std::vector<Enemy> enemies;
    sf::Clock clock;
    GameData gameData;

    Levels levels;
    std::vector<EnemyTiming> currentTimings;
    size_t nextSpawnIndex = 0;
    float levelElapsedTime = 0.0f;

    // Gacha reward state
    GachaReward currentReward;
    float gachaTimer = 0.0f;

    int grabbedEnemyIndex = -1;  // -1 means no enemy is grabbed
    const float GRAVITY = 1500.0f;  // Pixels per second squared
    const float DAMAGE_PER_PIXEL = 0.05f;  // Damage multiplier for fall distance
    const float X_FRICTION = 0.95f;  // Horizontal velocity damping (applied per 1/60th second)
    const float MIN_X = 0.0f;  // Left boundary
    const float MAX_X = 1200.0f;  // Right boundary
    const float CASTLE_EDGE_X = 900.0f;  // Left edge of castle wall
    const float ENEMY_FRONT_WIDTH = 25.0f;  // Body(20) + arm(5), multiplied by scale
    
    sf::Vector2f lastMousePos = {0.0f, 0.0f};  // Track mouse position for velocity
    sf::Vector2f throwVelocity = {0.0f, 0.0f};  // Accumulated throw velocity
    const float MAX_THROW_VELOCITY = 800.0f;  // Cap throw speed
    const float VELOCITY_DECAY = 0.85f;  // How much old velocity to keep (preserves momentum)

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (gameState == GameState::MainMenu){
                        if (buttons.isClicked(window, ButtonType::MAIN_MENU_PLAY, mousePosition)) {
                            gameState = GameState::Game;
                            render.startLevelIntro(gameData.level);
                            
                            currentTimings = levels.getEnemies(gameData.level);
                            nextSpawnIndex = 0;
                            levelElapsedTime = 0.0f;
                            enemies.clear();
                        }
                    }
                    else if (gameState == GameState::GameOver){
                        if (buttons.isClicked(window, ButtonType::GAME_OVER_MAIN_MENU, mousePosition)) {
                            gameState = GameState::MainMenu;
                            // Reset game data for a fresh start
                            gameData = GameData();
                            enemies.clear();
                            grabbedEnemyIndex = -1;
                            render.setTargetShade(0.78f);
                        }
                    }
                    else if (gameState == GameState::LevelComplete) {
                        // Only allow claim after card is fully revealed (2.3s)
                        if (gachaTimer >= 2.3f && buttons.isClicked(window, ButtonType::GACHA_CLAIM, mousePosition)) {
                            // Apply the reward
                            applyReward(gameData, currentReward);
                            
                            // Advance to next level
                            gameData.level++;
                            gameData.xp = 0;  // Reset XP for next level
                            
                            // Check if there are more levels
                            currentTimings = levels.getEnemies(gameData.level);
                            if (currentTimings.empty()) {
                                // No more levels - victory!
                                gameState = GameState::Victory;
                            } else {
                                gameState = GameState::Game;
                                render.startLevelIntro(gameData.level);
                                nextSpawnIndex = 0;
                                levelElapsedTime = 0.0f;
                                enemies.clear();
                                grabbedEnemyIndex = -1;
                            }
                        }
                    }
                    if (gameState == GameState::Game){
                        for (size_t i = 0; i < enemies.size(); ++i) {
                            Enemy& enemy = enemies[i];
                            if (enemy.state == EnemyState::Dead) continue;
                            
                            // Hitbox matches where enemy is drawn (position is top of head, body extends down)
                        sf::FloatRect hitbox(enemy.position.x - 10, enemy.position.y - 5, 40, 65);
                            if (hitbox.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
                                grabbedEnemyIndex = static_cast<int>(i);
                                enemy.state = EnemyState::Grabbed;
                                lastMousePos = {static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)};
                                throwVelocity = {0.0f, 0.0f};
                                break;
                            }
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left && grabbedEnemyIndex >= 0) {
                    Enemy& enemy = enemies[grabbedEnemyIndex];
                    
                    // Clamp velocity to reasonable throw speed
                    throwVelocity.x = std::clamp(throwVelocity.x, -MAX_THROW_VELOCITY, MAX_THROW_VELOCITY);
                    throwVelocity.y = std::clamp(throwVelocity.y, -MAX_THROW_VELOCITY, MAX_THROW_VELOCITY);
                    
                    // If dropped at ground level, check castle edge
                    if (enemy.position.y >= enemy.baseY - 5.0f) {
                        enemy.position.y = enemy.baseY;
                        float stopX = CASTLE_EDGE_X - ENEMY_FRONT_WIDTH * enemy.scale;
                        if (enemy.position.x >= stopX) {
                            // At or past castle edge - snap to edge and attack
                            enemy.position.x = stopX;
                            enemy.velocity = {0.0f, 0.0f};
                            enemy.state = EnemyState::Attacking;
                            enemy.attackTimer = 0.0f;
                            enemy.animationTime = 0.0f;
                        } else {
                            // Before castle - walk forward
                            enemy.state = EnemyState::Walking;
                            enemy.velocity = {throwVelocity.x, 0.0f};  // Keep horizontal momentum
                        }
                    } else {
                        enemy.state = EnemyState::Falling;
                        enemy.velocity = throwVelocity;  // Apply throw velocity
                    }
                    grabbedEnemyIndex = -1;
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                if (grabbedEnemyIndex >= 0) {
                    Enemy& enemy = enemies[grabbedEnemyIndex];
                    sf::Vector2f currentMousePos = {static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y)};
                    
                    // Calculate instant velocity from mouse movement
                    sf::Vector2f mouseDelta = currentMousePos - lastMousePos;
                    sf::Vector2f instantVelocity = mouseDelta * 80.0f;  // Scale to reasonable velocity
                    
                    // Blend with previous velocity to preserve momentum
                    // This means if you stop moving, velocity decays slowly instead of instantly zeroing
                    throwVelocity = throwVelocity * VELOCITY_DECAY + instantVelocity * (1.0f - VELOCITY_DECAY);
                    
                    // Update enemy position to follow mouse
                    enemy.position.x = std::clamp(currentMousePos.x, MIN_X, MAX_X);
                    enemy.position.y = std::min(currentMousePos.y, enemy.baseY);  // Can go up, but not below ground
                    
                    lastMousePos = currentMousePos;
                }
            }
        }

        // Update shade each frame
        render.updateShade(deltaTime);

        window.clear();

        if (gameState == GameState::MainMenu)
        {
            render.drawMainMenu(window, buttons);
        }
        else if (gameState == GameState::Game)
        {
            // Update level time and spawn enemies
            levelElapsedTime += deltaTime;
            checkAndSpawnEnemies(levelElapsedTime, currentTimings, nextSpawnIndex, enemies);

            // Update enemies
            for (Enemy& enemy : enemies)
            {
                if (enemy.state == EnemyState::Walking)
                {
                    // Apply any remaining horizontal velocity (from landing after throw)
                    if (std::abs(enemy.velocity.x) > 1.0f) {
                        enemy.position.x += enemy.velocity.x * deltaTime;
                        // Decay horizontal velocity while walking
                        enemy.velocity.x *= std::pow(0.90f, deltaTime * 60.0f);
                    } else {
                        // Normal walking toward castle
                        enemy.velocity.x = 0.0f;
                        enemy.position.x += enemy.speed * deltaTime;
                    }
                    
                    // Keep within bounds while walking
                    enemy.position.x = std::clamp(enemy.position.x, MIN_X, MAX_X);

                    // Check if enemy reached castle edge (front of enemy touches wall)
                    float stopX = CASTLE_EDGE_X - ENEMY_FRONT_WIDTH * enemy.scale;
                    if (enemy.position.x >= stopX) {
                        enemy.position.x = stopX;
                        enemy.velocity.x = 0.0f;
                        enemy.state = EnemyState::Attacking;
                        enemy.attackTimer = 0.0f;
                        enemy.animationTime = 0.0f;
                    }
                }
                else if (enemy.state == EnemyState::Attacking)
                {
                    // Deal periodic damage to the castle (reduced by damage resistance)
                    enemy.attackTimer += deltaTime;
                    if (enemy.attackTimer >= enemy.attackInterval) {
                        enemy.attackTimer -= enemy.attackInterval;
                        int damage = static_cast<int>(enemy.attackDamage * (1.0f - gameData.damageReduction));
                        if (damage < 1) damage = 1;  // Always deal at least 1 damage
                        gameData.currentHealth -= damage;
                        if (gameData.currentHealth < 0) gameData.currentHealth = 0;
                    }
                }
                else if (enemy.state == EnemyState::Falling)
                {
                    // Apply gravity to Y velocity
                    enemy.velocity.y += GRAVITY * deltaTime;
                    
                    // Apply friction to X velocity (frame-rate independent)
                    // Using pow to make it consistent regardless of framerate
                    enemy.velocity.x *= std::pow(X_FRICTION, deltaTime * 60.0f);
                    
                    // Update position
                    enemy.position.x += enemy.velocity.x * deltaTime;
                    enemy.position.y += enemy.velocity.y * deltaTime;
                    
                    // Clamp X to screen bounds and bounce off walls
                    if (enemy.position.x < MIN_X) {
                        enemy.position.x = MIN_X;
                        enemy.velocity.x = -enemy.velocity.x * 0.5f;  // Bounce with dampening
                    } else if (enemy.position.x > MAX_X) {
                        enemy.position.x = MAX_X;
                        enemy.velocity.x = -enemy.velocity.x * 0.5f;
                    }
                    
                    // Check if landed
                    if (enemy.position.y >= enemy.baseY)
                    {
                        // Calculate fall damage based on downward velocity
                        // Only count positive (downward) velocity for damage
                        float impactVelocity = std::max(0.0f, enemy.velocity.y);
                        // v² = 2gh, so equivalent height = v²/(2g)
                        float equivalentFallHeight = (impactVelocity * impactVelocity) / (2.0f * GRAVITY);
                        int damage = static_cast<int>(equivalentFallHeight * DAMAGE_PER_PIXEL);
                        
                        enemy.hp -= damage;
                        enemy.position.y = enemy.baseY;
                        // Keep horizontal velocity for sliding, only zero vertical
                        enemy.velocity.y = 0.0f;
                        
                        if (enemy.hp <= 0)
                        {
                            enemy.state = EnemyState::Dead;
                            enemy.deathTime = 0.0f;
                            // Award XP with multiplier
                            int xpGained = static_cast<int>(enemy.xpReward * gameData.xpMultiplier);
                            gameData.xp += xpGained;
                            gameData.totalXP += xpGained;
                        }
                        else
                        {
                            // Check position relative to castle edge
                            float stopX = CASTLE_EDGE_X - ENEMY_FRONT_WIDTH * enemy.scale;
                            if (enemy.position.x >= stopX) {
                                // Thrown past castle and survived - teleport back to castle edge
                                enemy.position.x = stopX;
                                enemy.velocity.x = 0.0f;
                                enemy.state = EnemyState::Attacking;
                                enemy.attackTimer = 0.0f;
                                enemy.animationTime = 0.0f;
                            } else {
                                // Thrown back before castle - resume walking
                                enemy.state = EnemyState::Walking;
                            }
                        }
                    }
                }
                else if (enemy.state == EnemyState::Dead)
                {
                    enemy.deathTime += deltaTime;
                }
            }

            // Check for game over
            if (gameData.currentHealth <= 0) {
                gameState = GameState::GameOver;
                grabbedEnemyIndex = -1;
            }

            // Check for level completion: all enemies spawned and all dead
            if (nextSpawnIndex >= currentTimings.size() && !enemies.empty()) {
                bool allDead = true;
                for (const Enemy& enemy : enemies) {
                    if (enemy.state != EnemyState::Dead) {
                        allDead = false;
                        break;
                    }
                }
                if (allDead) {
                    // Level complete! Transition to gacha reward screen
                    gameState = GameState::LevelComplete;
                    currentReward = rollGachaReward(gameData.level);
                    gachaTimer = 0.0f;
                    grabbedEnemyIndex = -1;
                }
            }
             
            render.drawGame(window, enemies, deltaTime, gameData.level, gameData);
        }
        else if (gameState == GameState::LevelComplete)
        {
            gachaTimer += deltaTime;
            render.drawLevelComplete(window, buttons, gameData, currentReward, gachaTimer);
        }
        else if (gameState == GameState::GameOver)
        {
            render.drawGameOver(window, buttons, gameData);
        }
        else if (gameState == GameState::Victory)
        {
            render.drawVictory(window, gameData);
        }

        window.display();
    }

    return 0;
}
