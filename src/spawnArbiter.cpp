#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "spawnManagement.cpp"
#include "UIArbiter.cpp"
#include "Phase2Screen.cpp"
#include "Phase3Screen.cpp"
#include "VictoryScreen.cpp"
//How to run:
/*g++ -Isrc/include -c src/main.cpp -o main.o
 g++ -Isrc/include -c src/Agent.cpp -o Agent.o
 g++ main.o Agent.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system
 ./main.exe*/

void spawnArbiter(
    sf::Clock& globalClock,
    float& lastSpawnTime,
    float& currentSpeed,
    float& currentMultiplier,
    std::vector<Agent>& Agents,
    sf::RenderWindow& window,
    float UIdepth,
    std::vector<int> killVecCTS
) {
    std::random_device rd;        // Seed source
    std::mt19937 gen(rd());       // Random number generator
    std::uniform_real_distribution<float> distRatio(0.0f, 1.0f); // Distribution for ratio selection
    std::uniform_real_distribution<float> distY(0.0f, static_cast<float>(window.getSize().y-UIdepth - 50)); // Random Y position

    // Spawn proportions
    float yelTriRat = 0.85f;       // Probability of spawning a yellow triangle
    float grnSqrRat = 0.15f;       // Probability of spawning a green square
    float bluChaser = 0.1;
    // Check if it's time to spawn
    if (willSpawn(globalClock, lastSpawnTime, currentSpeed, currentMultiplier)) {
        // Generate a random float to decide which agent to spawn
        float spawnChoice = distRatio(gen);
        float randomY = distY(gen); // Random Y position
        int score = scoreArbiter(killVecCTS[0],killVecCTS[1],killVecCTS[2]);
        if(score >= 25 && window.getSize().x < 1000){
            killVecCTS[0] += 1;
            currentMultiplier = 0.998;
            showPhase3(window);

            // Set the new window size
            sf::Vector2u newSize(1200, 600);
            window.setSize(newSize);

            // Adjust the view to maintain the aspect ratio
            sf::View view = window.getView();
            view.setSize(static_cast<float>(newSize.x), static_cast<float>(newSize.y));
            view.setCenter(newSize.x / 2.0f, newSize.y / 2.0f);
            window.setView(view);


        }
        if(score > 50){
            showVictoryScreen(window);
        }
        if (currentMultiplier == 1) {
            currentMultiplier = 0.999;
            showPhase2(window);

            // Set the new window size
            sf::Vector2u newSize(900, 450);
            window.setSize(newSize);

            // Adjust the view to maintain the aspect ratio
            sf::View view = window.getView();
            view.setSize(static_cast<float>(newSize.x), static_cast<float>(newSize.y));
            view.setCenter(newSize.x / 2.0f, newSize.y / 2.0f);
            window.setView(view);

            // Other logic
            auto circle = std::make_shared<sf::CircleShape>();
            spawn(Agents, false, true, 2, 100.f, circle, sf::Color::Blue, sf::Vector2f(300.f, randomY), 0.2f);
        }
        if(currentMultiplier == 0.999f){
            yelTriRat = 0.75;
            grnSqrRat = 0.1;
            bluChaser = 0.15;
        }
        if(currentMultiplier <= 0.998f){
            yelTriRat = 0.5;
            grnSqrRat = 0.0;
            bluChaser = 0.5;
            currentSpeed = 0.3;
        }
       // std::cout<< "curMult is " << currentMultiplier << "bluchaser is " << bluChaser << std::endl;

        if (spawnChoice <= yelTriRat) {
            // Spawn a yellow triangle
            auto triangle = std::make_shared<sf::ConvexShape>(3); // 3 sides for a triangle
            triangle->setPoint(0, sf::Vector2f(0.f, 0.f));        // Top point
            triangle->setPoint(1, sf::Vector2f(50.f, 100.f));     // Bottom-right point
            triangle->setPoint(2, sf::Vector2f(-50.f, 100.f));    // Bottom-left point
            triangle->setFillColor(sf::Color::Yellow);

            spawn(Agents, true, false, 2, 60.f, triangle, sf::Color::Yellow, sf::Vector2f(window.getSize().x + 50, randomY), 0.2f);
        } else if (spawnChoice <= yelTriRat + grnSqrRat) {
            // Spawn a green square
            auto square = std::make_shared<sf::RectangleShape>(sf::Vector2f(50.f, 50.f)); // Square size
            square->setFillColor(sf::Color::Green);

            spawn(Agents, true, false, 3, 50.f, square, sf::Color::Green, sf::Vector2f(window.getSize().x + 50, randomY), 0.2f);
        }
        else if (spawnChoice > yelTriRat + grnSqrRat){
            //auto circle = std::make_shared<sf::CircleShape>(sf::Vector2f(50.f, 50.f));
            //circle->setFillColor(sf::Color::Blue);
            //std::cout << "spawning chaser " << std::endl;
            spawn(Agents, false, true, 2, 20.f, std::make_shared<sf::CircleShape>(20), sf::Color::Blue, sf::Vector2f(window.getSize().x + 50, randomY), 0.2f);
            //std::cout << "spawned chaser " << std::endl;
        }
    }
}