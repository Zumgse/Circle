#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

void UIArbiter(
            float UIdepth,
            int score,
            float time,
            sf::Font& font,
            sf::RenderWindow& window
        ){
    sf::RectangleShape line(sf::Vector2f(window.getSize().x, 4)); // Line width equals window width
    line.setFillColor(sf::Color::White);                         // Line color
    line.setPosition(0, window.getSize().y - 100);               // Position 100 pixels from the bottom
    window.draw(line);
    
    static sf::Text scoreText;
    if (scoreText.getString() != std::to_string(score)) {
        if(window.getSize().x < 1000){
            scoreText.setString("Score: " + std::to_string(score)+ " / 25");
        }
        else{
            scoreText.setString("Score: " + std::to_string(score) + "/ 50");
        }
    }
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Red);
    scoreText.setPosition(0,line.getPosition().y);
    
    static sf::Text timeText;
    timeText.setPosition(0, scoreText.getPosition().y + scoreText.getCharacterSize());

    static float lastUpdateTime = -1.0f; // To track the last time the text was updated

// Initialize timeText once
if (lastUpdateTime < 0) {
    timeText.setFont(font);
    timeText.setCharacterSize(24);
    timeText.setFillColor(sf::Color::Red);
    scoreText.setPosition(0,line.getPosition().y);
    timeText.setPosition(0, scoreText.getPosition().y + scoreText.getCharacterSize());

}

// Update the timeText only if the condition is met
if (time >= lastUpdateTime + 1.0f) { // Check if at least 1 second has passed
    int wholeTime = static_cast<int>(time);
    timeText.setString("Time: " + std::to_string(wholeTime));
    lastUpdateTime = time; // Update the last update time
}

// Draw the timeText
window.draw(timeText);

window.draw(scoreText);
}