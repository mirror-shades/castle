
#include <SFML/Graphics.hpp>
#include "render.h"
#include "enemy.h"
#include "state.h"
#include "buttons.h"
#include "core.h"
#include "levels.h"
#include "warrior.h"
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
    std::vector<FireArrowProjectile> fireArrows;
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
    const sf::Vector2f FIRE_ARCHER_POS = {930.0f, 266.0f};
    const float FIRE_ARCHER_ATTACK_INTERVAL = 1.0f;
    const float FIRE_ARROW_SPEED = 700.0f;
    float fireArcherAttackTimer = 0.0f;

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
                            fireArrows.clear();
                            fireArcherAttackTimer = 0.0f;
                        }
                    }
                    else if (gameState == GameState::GameOver){
                        if (buttons.isClicked(window, ButtonType::GAME_OVER_MAIN_MENU, mousePosition)) {
                            gameState = GameState::MainMenu;
                            // Reset game data for a fresh start
                            gameData = GameData();
                            enemies.clear();
                            fireArrows.clear();
                            grabbedEnemyIndex = -1;
                            render.setTargetShade(0.78f);
                            fireArcherAttackTimer = 0.0f;
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
                                fireArrows.clear();
                                grabbedEnemyIndex = -1;
                                fireArcherAttackTimer = 0.0f;
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

            auto killEnemy = [&](Enemy& enemy) {
                if (enemy.state == EnemyState::Dead) return;
                enemy.state = EnemyState::Dead;
                enemy.deathTime = 0.0f;
                int xpGained = static_cast<int>(enemy.xpReward * gameData.xpMultiplier);
                gameData.xp += xpGained;
                gameData.totalXP += xpGained;
            };

            auto enemyEffectiveVelocity = [](const Enemy& enemy) -> sf::Vector2f {
                if (enemy.state == EnemyState::Falling) {
                    return enemy.velocity;
                }
                if (enemy.state == EnemyState::Walking) {
                    // Most walkers advance with speed directly, not velocity.x.
                    float walkVx = (std::abs(enemy.velocity.x) > 1.0f) ? enemy.velocity.x : enemy.speed;
                    return {walkVx, 0.0f};
                }
                return {0.0f, 0.0f};
            };

            auto computeInterceptDirection = [](sf::Vector2f shooterPos,
                                                sf::Vector2f targetPos,
                                                sf::Vector2f targetVel,
                                                float projectileSpeed,
                                                sf::Vector2f& outDir) -> bool {
                sf::Vector2f r = targetPos - shooterPos;
                float a = targetVel.x * targetVel.x + targetVel.y * targetVel.y - projectileSpeed * projectileSpeed;
                float b = 2.0f * (r.x * targetVel.x + r.y * targetVel.y);
                float c = r.x * r.x + r.y * r.y;

                float t = -1.0f;
                if (std::abs(a) < 1e-4f) {
                    if (std::abs(b) < 1e-4f) return false;
                    t = -c / b;
                } else {
                    float disc = b * b - 4.0f * a * c;
                    if (disc < 0.0f) return false;
                    float sqrtDisc = std::sqrt(disc);
                    float t1 = (-b - sqrtDisc) / (2.0f * a);
                    float t2 = (-b + sqrtDisc) / (2.0f * a);
                    if (t1 > 0.0f && t2 > 0.0f) t = std::min(t1, t2);
                    else if (t1 > 0.0f) t = t1;
                    else if (t2 > 0.0f) t = t2;
                }
                if (t <= 0.0f) return false;

                sf::Vector2f aimPoint = targetPos + targetVel * t;
                sf::Vector2f dir = aimPoint - shooterPos;
                float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                if (len <= 0.001f) return false;
                outDir = dir / len;
                return true;
            };

            auto lineIntersectsRect = [](sf::Vector2f p0, sf::Vector2f p1, const sf::FloatRect& rect) -> bool {
                float t0 = 0.0f;
                float t1 = 1.0f;
                float dx = p1.x - p0.x;
                float dy = p1.y - p0.y;

                auto clip = [&](float p, float q) -> bool {
                    if (std::abs(p) < 1e-6f) return q >= 0.0f;
                    float r = q / p;
                    if (p < 0.0f) {
                        if (r > t1) return false;
                        if (r > t0) t0 = r;
                    } else {
                        if (r < t0) return false;
                        if (r < t1) t1 = r;
                    }
                    return true;
                };

                return clip(-dx, p0.x - rect.left) &&
                       clip(dx, rect.left + rect.width - p0.x) &&
                       clip(-dy, p0.y - rect.top) &&
                       clip(dy, rect.top + rect.height - p0.y) &&
                       t1 >= t0;
            };

            // Fire Archer auto-attacks the nearest living enemy.
            fireArcherAttackTimer += deltaTime;
            if (fireArcherAttackTimer >= FIRE_ARCHER_ATTACK_INTERVAL) {
                int nearestEnemyIndex = -1;
                float nearestDistSq = 0.0f;
                for (size_t i = 0; i < enemies.size(); ++i) {
                    if (enemies[i].state == EnemyState::Dead) continue;
                    sf::Vector2f toEnemy = enemies[i].position - FIRE_ARCHER_POS;
                    float distSq = toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y;
                    if (nearestEnemyIndex == -1 || distSq < nearestDistSq) {
                        nearestEnemyIndex = static_cast<int>(i);
                        nearestDistSq = distSq;
                    }
                }

                if (nearestEnemyIndex >= 0) {
                    fireArcherAttackTimer = 0.0f;
                    const Enemy& target = enemies[nearestEnemyIndex];
                    sf::Vector2f targetVel = enemyEffectiveVelocity(target);
                    sf::Vector2f direction;

                    bool hasLead = computeInterceptDirection(
                        FIRE_ARCHER_POS, target.position, targetVel, FIRE_ARROW_SPEED, direction);

                    if (!hasLead) {
                        sf::Vector2f toEnemy = target.position - FIRE_ARCHER_POS;
                        float length = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);
                        if (length > 0.001f) {
                            direction = toEnemy / length;
                            hasLead = true;
                        }
                    }

                    if (hasLead) {
                        FireArrowProjectile arrow;
                        arrow.position = FIRE_ARCHER_POS;
                        arrow.velocity = direction * FIRE_ARROW_SPEED;
                        fireArrows.push_back(arrow);
                    }
                }
            }

            // Update Fire Archer projectiles and apply hits.
            for (size_t i = 0; i < fireArrows.size();) {
                FireArrowProjectile& arrow = fireArrows[i];
                sf::Vector2f prevPos = arrow.position;
                arrow.position += arrow.velocity * deltaTime;

                bool consumed = false;
                for (Enemy& enemy : enemies) {
                    if (enemy.state == EnemyState::Dead) continue;
                    sf::FloatRect hitbox(enemy.position.x - 10.0f, enemy.position.y - 5.0f, 40.0f, 65.0f);
                    if (hitbox.contains(prevPos) || hitbox.contains(arrow.position) ||
                        lineIntersectsRect(prevPos, arrow.position, hitbox)) {
                        enemy.hp -= arrow.directDamage;
                        enemy.burnDps = std::max(enemy.burnDps, arrow.burnDps);
                        enemy.burnTimeRemaining = std::max(enemy.burnTimeRemaining, arrow.burnDuration);
                        if (enemy.hp <= 0) {
                            killEnemy(enemy);
                        }
                        consumed = true;
                        break;
                    }
                }

                if (!consumed) {
                    if (arrow.position.x < -50.0f || arrow.position.x > 1250.0f ||
                        arrow.position.y < -50.0f || arrow.position.y > 770.0f) {
                        consumed = true;
                    }
                }

                if (consumed) {
                    fireArrows.erase(fireArrows.begin() + static_cast<long long>(i));
                } else {
                    ++i;
                }
            }

            // Update enemies
            for (Enemy& enemy : enemies)
            {
                if (enemy.state != EnemyState::Dead && enemy.burnTimeRemaining > 0.0f) {
                    enemy.burnTimeRemaining = std::max(0.0f, enemy.burnTimeRemaining - deltaTime);
                    enemy.burnDamageBuffer += enemy.burnDps * deltaTime;
                    int burnDamage = static_cast<int>(enemy.burnDamageBuffer);
                    if (burnDamage > 0) {
                        enemy.burnDamageBuffer -= static_cast<float>(burnDamage);
                        enemy.hp -= burnDamage;
                        if (enemy.hp <= 0) {
                            killEnemy(enemy);
                        }
                    }
                    if (enemy.burnTimeRemaining <= 0.0f) {
                        enemy.burnDps = 0.0f;
                        enemy.burnDamageBuffer = 0.0f;
                    }
                }

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
                            killEnemy(enemy);
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
                    fireArrows.clear();
                }
            }
             
            render.drawGame(window, enemies, fireArrows, deltaTime, gameData.level, gameData);
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
