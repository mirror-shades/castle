#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include "render.h"
#include "enemy.h"
#include "buttons.h"

Render::Render() 
{
    font.loadFromFile("assets/arial.ttf");
}

void Render::drawGradient(sf::RenderWindow& window, int startY, int endY,
                          sf::Color startColor, sf::Color endColor) 
{
    float width = static_cast<float>(window.getSize().x);
    int range = endY - startY;
    
    for (int i = startY; i < endY; i += 20) {
        float t = static_cast<float>(i - startY) / range;
        sf::RectangleShape layer(sf::Vector2f(width, 20));
        layer.setPosition(0, static_cast<float>(i));
        layer.setFillColor(sf::Color(
            static_cast<sf::Uint8>(startColor.r + (endColor.r - startColor.r) * t),
            static_cast<sf::Uint8>(startColor.g + (endColor.g - startColor.g) * t),
            static_cast<sf::Uint8>(startColor.b + (endColor.b - startColor.b) * t)
        ));
        window.draw(layer);
    }
}

void Render::drawCenteredText(sf::RenderWindow& window, const std::string& str,
                               unsigned int size, float y, sf::Color color) 
{
    sf::Text text(str, font, size);
    text.setFillColor(color);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(window.getSize().x / 2.0f, y);
    window.draw(text);
}

void Render::drawBackground(sf::RenderWindow& window) 
{
    float width = static_cast<float>(window.getSize().x);
    
    // Sky gradient
    drawGradient(window, 0, 470, sf::Color(100, 150, 200), sf::Color(135, 206, 235));
    
    // Grass divider line
    sf::RectangleShape grassLine(sf::Vector2f(width, 20));
    grassLine.setPosition(0, 470);
    grassLine.setOutlineColor(sf::Color::Black);
    grassLine.setOutlineThickness(3.f);
    grassLine.setFillColor(sf::Color(60, 130, 0));
    window.draw(grassLine);
    
    // Ground gradient
    drawGradient(window, 470, 720, sf::Color(60, 130, 0), sf::Color(76, 153, 0));
}

void Render::drawHealthBar(sf::RenderWindow& window, int currentHealth, int maxHealth)
{
    float healthBarWidth = 250.0f;

    sf::RectangleShape healthBarBackground(sf::Vector2f(250, 40));
    healthBarBackground.setFillColor(sf::Color(155, 155, 155));
    healthBarBackground.setOutlineColor(sf::Color::Black);
    healthBarBackground.setOutlineThickness(3.f);
    healthBarBackground.setPosition(900, 200);
    window.draw(healthBarBackground);

    sf::RectangleShape healthBar(sf::Vector2f(healthBarWidth * (static_cast<float>(currentHealth) / static_cast<float>(maxHealth)), 40));
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setPosition(900, 200);
    window.draw(healthBar);
}

void Render::drawXPBar(sf::RenderWindow& window, GameData& gameData)
{
    float barWidth = 200.0f;
    float barHeight = 20.0f;
    float barX = 20.0f;
    float barY = 15.0f;

    // XP label
    sf::Text xpLabel("XP", font, 20);
    xpLabel.setFillColor(sf::Color(180, 130, 255));
    xpLabel.setOutlineColor(sf::Color::Black);
    xpLabel.setOutlineThickness(1.5f);
    xpLabel.setPosition(barX, barY - 2.0f);
    window.draw(xpLabel);

    float labelOffset = 40.0f;

    // Background
    sf::RectangleShape bgBar(sf::Vector2f(barWidth, barHeight));
    bgBar.setFillColor(sf::Color(40, 40, 60));
    bgBar.setOutlineColor(sf::Color(100, 80, 160));
    bgBar.setOutlineThickness(2.f);
    bgBar.setPosition(barX + labelOffset, barY);
    window.draw(bgBar);

    // Fill based on XP progress within current level
    // XP needed per level scales: level * 50
    int xpForLevel = gameData.level * 50;
    float fillRatio = std::min(1.0f, static_cast<float>(gameData.xp) / static_cast<float>(xpForLevel));

    sf::RectangleShape fillBar(sf::Vector2f(barWidth * fillRatio, barHeight));
    fillBar.setFillColor(sf::Color(150, 100, 255));
    fillBar.setPosition(barX + labelOffset, barY);
    window.draw(fillBar);

    // XP text on bar
    sf::Text xpText(std::to_string(gameData.xp) + " / " + std::to_string(xpForLevel), font, 14);
    xpText.setFillColor(sf::Color::White);
    xpText.setOutlineColor(sf::Color::Black);
    xpText.setOutlineThickness(1.f);
    sf::FloatRect textBounds = xpText.getLocalBounds();
    xpText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    xpText.setPosition(barX + labelOffset + barWidth / 2.0f, barY + barHeight / 2.0f);
    window.draw(xpText);

    // Level indicator
    sf::Text lvlText("Lv." + std::to_string(gameData.level), font, 18);
    lvlText.setFillColor(sf::Color(255, 215, 0));
    lvlText.setOutlineColor(sf::Color::Black);
    lvlText.setOutlineThickness(1.5f);
    lvlText.setPosition(barX + labelOffset + barWidth + 10.0f, barY - 1.0f);
    window.draw(lvlText);
}

void Render::setTargetShade(float target)
{
    targetShade = target;
    currentShade = target;
}

bool Render::isShadeComplete() const
{
    return currentShade == targetShade;
}

void Render::updateShade(float deltaTime) 
{
    if (currentShade < targetShade)
        currentShade = std::min(currentShade + shadeSpeed * deltaTime, targetShade);
    else if (currentShade > targetShade)
        currentShade = std::max(currentShade - shadeSpeed * deltaTime, targetShade);
}

void Render::drawShade(sf::RenderWindow& window) 
{
    if (currentShade <= 0.0f) return;
    
    sf::RectangleShape shade(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    shade.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(currentShade * 255.0f)));
    window.draw(shade);
}

void Render::drawMainMenu(sf::RenderWindow& window, Buttons& buttons) 
{
    drawBackground(window);
    drawCastle(window);
    drawShade(window);
    
    float centerY = window.getSize().y / 2.0f;
    drawCenteredText(window, "Castle Defense", 100, centerY - 100, sf::Color::White);
    
    sf::RectangleShape playButton(buttons.getSize(ButtonType::MAIN_MENU_PLAY));
    playButton.setFillColor(sf::Color::White);
    playButton.setPosition(buttons.getPosition(window, ButtonType::MAIN_MENU_PLAY));
    window.draw(playButton);


    drawCenteredText(window, "Play", 50, centerY + 100, sf::Color::Black);
}

void Render::drawGameOver(sf::RenderWindow& window, Buttons& buttons, GameData& gameData) 
{
    drawBackground(window);
    drawCastle(window);

    // Dark overlay
    sf::RectangleShape overlay(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    float centerY = window.getSize().y / 2.0f;

    // "You Lose" title
    drawCenteredText(window, "You Lose", 100, centerY - 100, sf::Color(200, 30, 30));

    // XP earned text
    drawCenteredText(window, "Total XP: " + std::to_string(gameData.totalXP), 40, centerY, sf::Color(180, 130, 255));

    // Main menu button
    sf::RectangleShape menuButton(buttons.getSize(ButtonType::GAME_OVER_MAIN_MENU));
    menuButton.setFillColor(sf::Color::White);
    menuButton.setPosition(buttons.getPosition(window, ButtonType::GAME_OVER_MAIN_MENU));
    window.draw(menuButton);

    sf::Vector2f btnPos = buttons.getPosition(window, ButtonType::GAME_OVER_MAIN_MENU);
    sf::Vector2f btnSize = buttons.getSize(ButtonType::GAME_OVER_MAIN_MENU);
    drawCenteredText(window, "Main Menu", 40, btnPos.y + btnSize.y / 2.0f, sf::Color::Black);
}

void Render::drawLevelComplete(sf::RenderWindow& window, Buttons& buttons, 
                                GameData& gameData, const GachaReward& reward, float timer)
{
    drawBackground(window);
    drawCastle(window);

    // Dark overlay
    sf::RectangleShape overlay(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    float centerX = window.getSize().x / 2.0f;
    float centerY = window.getSize().y / 2.0f;

    // "Level Complete!" title with fade-in
    float titleAlpha = std::min(1.0f, timer / 0.5f);
    sf::Color titleColor(255, 215, 0, static_cast<sf::Uint8>(titleAlpha * 255));
    drawCenteredText(window, "Level " + std::to_string(gameData.level) + " Complete!", 
                     70, centerY - 200, titleColor);

    // Gacha card animation
    float cardRevealTime = 1.0f;   // Card starts revealing at this time
    float cardDoneTime = 2.0f;     // Card fully revealed

    if (timer >= cardRevealTime) {
        float cardProgress = std::min(1.0f, (timer - cardRevealTime) / (cardDoneTime - cardRevealTime));
        
        // Eased progress for smooth reveal
        float eased = cardProgress * cardProgress * (3.0f - 2.0f * cardProgress);

        // Card dimensions
        float cardW = 300.0f;
        float cardH = 200.0f;
        float cardX = centerX - cardW / 2.0f;
        float cardY = centerY - cardH / 2.0f - 30.0f;

        // Card scaling animation (starts small, grows to full)
        float scaleAmt = 0.5f + 0.5f * eased;
        
        // Rarity colors
        sf::Color rarityColor;
        std::string rarityText;
        switch (reward.rarity) {
            case 3:  rarityColor = sf::Color(200, 130, 255); rarityText = "EPIC";   break;
            case 2:  rarityColor = sf::Color(80, 160, 255);  rarityText = "RARE";   break;
            default: rarityColor = sf::Color(180, 180, 180); rarityText = "COMMON"; break;
        }

        // Card glow (rarity colored)
        sf::RectangleShape glow(sf::Vector2f((cardW + 16) * scaleAmt, (cardH + 16) * scaleAmt));
        glow.setOrigin((cardW + 16) * scaleAmt / 2.0f, (cardH + 16) * scaleAmt / 2.0f);
        glow.setPosition(centerX, centerY - 30.0f);
        glow.setFillColor(sf::Color(rarityColor.r, rarityColor.g, rarityColor.b, 
                          static_cast<sf::Uint8>(100 * eased)));
        window.draw(glow);

        // Card background
        sf::RectangleShape card(sf::Vector2f(cardW * scaleAmt, cardH * scaleAmt));
        card.setOrigin(cardW * scaleAmt / 2.0f, cardH * scaleAmt / 2.0f);
        card.setPosition(centerX, centerY - 30.0f);
        card.setFillColor(sf::Color(30, 30, 50, static_cast<sf::Uint8>(240 * eased)));
        card.setOutlineColor(sf::Color(rarityColor.r, rarityColor.g, rarityColor.b,
                             static_cast<sf::Uint8>(255 * eased)));
        card.setOutlineThickness(3.f);
        window.draw(card);

        // Card content (only after card is mostly revealed)
        if (cardProgress > 0.5f) {
            float contentAlpha = std::min(1.0f, (cardProgress - 0.5f) / 0.5f);
            sf::Uint8 alpha = static_cast<sf::Uint8>(contentAlpha * 255);

            // Rarity label
            drawCenteredText(window, rarityText, 20, centerY - 90.0f,
                           sf::Color(rarityColor.r, rarityColor.g, rarityColor.b, alpha));

            // Reward name
            drawCenteredText(window, reward.name, 32, centerY - 50.0f,
                           sf::Color(255, 255, 255, alpha));

            // Reward description
            drawCenteredText(window, reward.description, 20, centerY - 10.0f,
                           sf::Color(200, 200, 200, alpha));
        }
    }
    else {
        // Before card reveal: show spinning "?" effect
        float spinSpeed = 8.0f;
        float pulse = std::sin(timer * spinSpeed) * 0.3f + 0.7f;
        sf::Uint8 qAlpha = static_cast<sf::Uint8>(pulse * 255);
        drawCenteredText(window, "?", 120, centerY - 30.0f, sf::Color(255, 215, 0, qAlpha));
    }

    // Claim button (only after card is fully revealed)
    if (timer >= cardDoneTime + 0.3f) {
        float btnAlpha = std::min(1.0f, (timer - cardDoneTime - 0.3f) / 0.3f);
        
        sf::RectangleShape claimBtn(buttons.getSize(ButtonType::GACHA_CLAIM));
        claimBtn.setFillColor(sf::Color(100, 200, 100, static_cast<sf::Uint8>(btnAlpha * 255)));
        claimBtn.setOutlineColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(btnAlpha * 255)));
        claimBtn.setOutlineThickness(2.f);
        claimBtn.setPosition(buttons.getPosition(window, ButtonType::GACHA_CLAIM));
        window.draw(claimBtn);

        sf::Vector2f btnPos = buttons.getPosition(window, ButtonType::GACHA_CLAIM);
        sf::Vector2f btnSize = buttons.getSize(ButtonType::GACHA_CLAIM);
        drawCenteredText(window, "Claim & Continue", 30, 
                        btnPos.y + btnSize.y / 2.0f, 
                        sf::Color(255, 255, 255, static_cast<sf::Uint8>(btnAlpha * 255)));
    }
}

void Render::drawVictory(sf::RenderWindow& window, GameData& gameData)
{
    drawBackground(window);
    drawCastle(window);

    sf::RectangleShape overlay(sf::Vector2f(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    ));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    drawCenteredText(window, "Victory!", 100, 260, sf::Color(255, 215, 0));
    drawCenteredText(window, "All levels cleared!", 40, 360, sf::Color::White);
    drawCenteredText(window, "Total XP: " + std::to_string(gameData.totalXP), 36, 420, sf::Color(180, 130, 255));
}

void Render::startLevelIntro(int level) 
{
    currentLevel = level;
    levelIntroTimer = 0.0f;
    levelIntroActive = true;
}

void Render::drawLevelIntro(sf::RenderWindow& window, float deltaTime) 
{
    if (!levelIntroActive) return;
    
    levelIntroTimer += deltaTime;
    
    float textOpacity = 0.0f;
    if (levelIntroTimer < INTRO_FADE_IN_END) {
        textOpacity = levelIntroTimer / INTRO_FADE_IN_END;
    } else if (levelIntroTimer < INTRO_HOLD_END) {
        textOpacity = 1.0f;
    } else if (levelIntroTimer < INTRO_FADE_OUT_END) {
        textOpacity = 1.0f - (levelIntroTimer - INTRO_HOLD_END) / (INTRO_FADE_OUT_END - INTRO_HOLD_END);
    } else {
        targetShade = std::max(0.0f, std::min(1.0f, 0.0f));
        levelIntroActive = false;
        return;
    }
    
    sf::Color color(255, 255, 255, static_cast<sf::Uint8>(textOpacity * 255));
    drawCenteredText(window, "Level " + std::to_string(currentLevel), 80, 
                     window.getSize().y / 2.0f, color);
}

void Render::drawGame(sf::RenderWindow& window, std::vector<Enemy>& enemies, 
                       float deltaTime, int level, GameData& gameData) 
{
    for (Enemy& enemy : enemies)
        enemy.animationTime += deltaTime;
    
    drawBackground(window);
    drawCastle(window);
    drawHealthBar(window, gameData.currentHealth, gameData.maxHealth);

    // Sort enemies by baseY so those closer to the bottom render on top
    std::vector<Enemy*> sortedEnemies;
    sortedEnemies.reserve(enemies.size());
    for (Enemy& enemy : enemies)
        sortedEnemies.push_back(&enemy);
    std::sort(sortedEnemies.begin(), sortedEnemies.end(),
              [](const Enemy* a, const Enemy* b) { return a->baseY < b->baseY; });
    for (Enemy* enemy : sortedEnemies)
        drawEnemy(window, *enemy);

    // XP bar display (top-left HUD)
    drawXPBar(window, gameData);

    drawShade(window);
    drawLevelIntro(window, deltaTime);
}

void Render::drawCastle(sf::RenderWindow& window) 
{
    // Main wall
    sf::RectangleShape base(sf::Vector2f(300, 400));
    base.setFillColor(sf::Color(128, 128, 128));
    base.setOutlineColor(sf::Color::Black);
    base.setOutlineThickness(3.f);
    base.setPosition(900, 300);
    window.draw(base);

    // Horizontal brick lines
    for (int i = 0; i < 7; i++) {
        sf::RectangleShape line(sf::Vector2f(300, 3));
        line.setFillColor(sf::Color(180, 180, 180));
        line.setPosition(900, 350 + i * 50);
        window.draw(line);
    }

    // Vertical brick lines (staggered)
    for (int row = 0; row < 8; row++) {
        int offset = (row % 2 == 0) ? 25 : 0;
        for (int col = 0; col < 8; col++) {
            sf::RectangleShape line(sf::Vector2f(3, 50));
            line.setFillColor(sf::Color(180, 180, 180));
            line.setPosition(925 + col * 50 + offset, 300 + row * 50);
            window.draw(line);
        }
    }

    // Wooden gate
    sf::RectangleShape gate(sf::Vector2f(100, 300));
    gate.setFillColor(sf::Color(139, 69, 19));
    gate.setOutlineColor(sf::Color::Black);
    gate.setOutlineThickness(3.f);
    gate.setPosition(900, 400);
    window.draw(gate);

    // Gate boards
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape board(sf::Vector2f(5, 300));
        board.setFillColor(sf::Color(159, 89, 39));
        board.setPosition(925 + i * 25, 400);
        window.draw(board);
    }

    // Gate braces
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape brace(sf::Vector2f(100, 20));
        brace.setFillColor(sf::Color::Black);
        brace.setPosition(900, 450 + i * 80);
        window.draw(brace);
    }
}

void Render::drawEnemy(sf::RenderWindow& window, Enemy& enemy) 
{
    // Skip enemies whose death animation is fully complete
    if (enemy.state == EnemyState::Dead && enemy.deathTime > 2.1f) return;

    float x = enemy.position.x;
    float y = enemy.position.y;
    
    // Type-specific properties
    sf::Color color;
    float scale = 1.0f;
    switch (enemy.type) {
        case EnemyType::Goblin:   color = sf::Color(0, 180, 0);     scale = 0.8f;  break;
        case EnemyType::Orc:      color = sf::Color(100, 150, 80);  scale = 1.2f;  break;
        case EnemyType::Skeleton: color = sf::Color(220, 220, 200); scale = 1.0f;  break;
        case EnemyType::Troll:    color = sf::Color(80, 80, 120);   scale = 1.8f;  break;
    }
    
    // Death animation parameters
    const float FALL_DURATION = 0.5f;   // Time to tip over
    const float SINK_DELAY = 0.7f;      // Pause on ground before sinking
    const float SINK_DURATION = 1.2f;   // Time to sink and fade away
    
    float deathRotation = 0.0f;
    float sinkOffset = 0.0f;
    sf::Uint8 deathAlpha = 255;
    
    if (enemy.state == EnemyState::Dead) {
        // Phase 1: Tip over (rotate from 0 to 90 degrees)
        if (enemy.deathTime < FALL_DURATION) {
            float t = enemy.deathTime / FALL_DURATION;
            t = t * t;  // Ease-in for gravity-like acceleration
            deathRotation = 90.0f * t;
        } else {
            deathRotation = 90.0f;
        }
        
        // Phase 2: Sink into the ground + fade out
        if (enemy.deathTime > SINK_DELAY) {
            float sinkT = std::min(1.0f, (enemy.deathTime - SINK_DELAY) / SINK_DURATION);
            sinkT = sinkT * sinkT * (3.0f - 2.0f * sinkT);  // Smoothstep easing
            sinkOffset = 60.0f * scale * sinkT;
            deathAlpha = static_cast<sf::Uint8>(255.0f * (1.0f - sinkT));
        }
        
        // Darken color for a dead look
        color.r = static_cast<sf::Uint8>(color.r * 0.6f);
        color.g = static_cast<sf::Uint8>(color.g * 0.6f);
        color.b = static_cast<sf::Uint8>(color.b * 0.6f);
    }
    
    // Helper to apply death alpha to any color
    auto withAlpha = [deathAlpha](sf::Color c) -> sf::Color {
        c.a = deathAlpha;
        return c;
    };
    
    // Build render states with rotation transform for death animation
    sf::RenderStates states;
    if (enemy.state == EnemyState::Dead) {
        // Rotate around the enemy's feet (bottom-center)
        float pivotX = x + 10.0f * scale;
        float pivotY = y + 56.0f * scale;
        
        sf::Transform transform;
        transform.translate(pivotX, pivotY + sinkOffset);
        transform.rotate(deathRotation);
        transform.translate(-pivotX, -pivotY);
        states.transform = transform;
    }
    
    // Animation state
    float legLOffset = 0, legROffset = 0;
    float armLOffsetX = 0, armLOffsetY = 0;
    float armROffsetX = 0, armROffsetY = 0;
    float bodyBob = 0;
    
    if (enemy.state == EnemyState::Walking) {
        float cycle = std::sin(enemy.animationTime * 8.0f);
        legLOffset = cycle * 6.0f * scale;
        legROffset = -cycle * 6.0f * scale;
        bodyBob = std::abs(cycle) * 2.0f * scale;
        armLOffsetY = -cycle * 4.0f * scale;
        armROffsetY = cycle * 4.0f * scale;
    }
    else if (enemy.state == EnemyState::Attacking) {
        float phase = std::fmod(enemy.animationTime * 3.0f, 1.0f);
        
        if (phase < 0.3f) {  // Wind up
            float t = phase / 0.3f;
            armROffsetX = -8.0f * scale * t;
            armROffsetY = -5.0f * scale * t;
            armLOffsetX = -4.0f * scale * t;
        } else if (phase < 0.5f) {  // Strike
            float t = (phase - 0.3f) / 0.2f;
            armROffsetX = -8.0f * scale + 20.0f * scale * t;
            armROffsetY = -5.0f * scale + 10.0f * scale * t;
            armLOffsetX = -4.0f * scale + 10.0f * scale * t;
        } else {  // Recover
            float t = (phase - 0.5f) / 0.5f;
            armROffsetX = 12.0f * scale * (1.0f - t);
            armROffsetY = 5.0f * scale * (1.0f - t);
            armLOffsetX = 6.0f * scale * (1.0f - t);
        }
        bodyBob = std::sin(phase * 3.14159f) * 3.0f * scale;
    }
    // Dead state: all offsets stay 0 (neutral pose, frozen in place)
    
    float adjustedY = y - bodyBob;
    sf::Color legColor = withAlpha(sf::Color(
        static_cast<sf::Uint8>(color.r * 0.7f),
        static_cast<sf::Uint8>(color.g * 0.7f),
        static_cast<sf::Uint8>(color.b * 0.7f)));
    
    // Draw body parts (back to front)
    sf::RectangleShape part;
    
    // Left leg
    part.setSize(sf::Vector2f(8 * scale, 15 * scale));
    part.setFillColor(legColor);
    part.setPosition(x + legLOffset, y + 41 * scale);
    window.draw(part, states);
    
    // Right leg
    part.setPosition(x + 12 * scale + legROffset, y + 41 * scale);
    window.draw(part, states);
    
    // Body
    part.setSize(sf::Vector2f(20 * scale, 25 * scale));
    part.setFillColor(withAlpha(color));
    part.setPosition(x, adjustedY + 16 * scale);
    window.draw(part, states);
    
    // Left arm
    part.setSize(sf::Vector2f(5 * scale, 18 * scale));
    part.setFillColor(withAlpha(color));
    part.setPosition(x - 5 * scale + armLOffsetX, adjustedY + 18 * scale + armLOffsetY);
    window.draw(part, states);
    
    // Right arm
    part.setPosition(x + 20 * scale + armROffsetX, adjustedY + 18 * scale + armROffsetY);
    window.draw(part, states);
    
    // Head
    part.setSize(sf::Vector2f(16 * scale, 16 * scale));
    part.setFillColor(withAlpha(color));
    part.setPosition(x + 2 * scale, adjustedY);
    window.draw(part, states);
}
