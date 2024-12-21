#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip> // For std::setprecision
#include <sstream> // For std::ostringstream
#include "scoreArbiter.cpp"
inline void showGameOver(sf::RenderWindow& window, std::vector<int>& killVecCTS) {
    // Load a font
    sf::Font font;
    if (!font.loadFromFile("C:/Users/Ryan/Games/RyanMadeGames/Proj1/src/Khamenet-8OLn0.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }
    int score = scoreArbiter(killVecCTS[0],killVecCTS[1],killVecCTS[2]);
    // Create the "Game Over" text
    sf::Text gameOverText;
    gameOverText.setFont(font);
    
     std::ostringstream speedStream;
    speedStream << std::fixed << std::setprecision(2) << score;
    gameOverText.setString("LOSS\nScore: " + speedStream.str());

    gameOverText.setCharacterSize(100); // Size of the text
    gameOverText.setFillColor(sf::Color::Red); // Text color
    gameOverText.setStyle(sf::Text::Bold);

    // Center the text
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textBounds.width / 2, textBounds.height / 2);
    gameOverText.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    // Display the text
    window.clear(); // Clear the window
    window.draw(gameOverText); // Draw the text
    window.display(); // Display the changes

    // Pause for user to see the "Game Over" message
    sf::sleep(sf::seconds(5)); // Pause for 300 seconds
    std::exit(0);
}
