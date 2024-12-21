#include <SFML/Graphics.hpp>
#include "Agent.cpp"
#include <cmath>
#include <iostream>
//How to run:
/*g++ -Isrc/include -c src/main.cpp -o main.o
 g++ -Isrc/include -c src/Agent.cpp -o Agent.o
 g++ main.o Agent.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system
 ./main.exe*/

inline bool willSpawn(sf::Clock& globalClock, float& lastSpawn, float& currentSpeed, float& currentMultiplier) {
    float elapsedTime = globalClock.getElapsedTime().asSeconds();

    // Gradually decreasing spawn interval as time progresses
    float spawnInterval = std::max(
        5.0f / (1.0f + elapsedTime * 0.1f), // Slowly decreases as elapsed time increases
        0.5f / std::sqrt(currentSpeed)      // Minimum spawn interval based on current speed
    );

    // Check if enough time has passed since the last spawn
    if (elapsedTime - lastSpawn >= spawnInterval) {
        lastSpawn = elapsedTime;        // Update the last spawn time
        currentSpeed *= currentMultiplier;          // Gradually increase the current speed
       // std::cout << currentSpeed << std::endl;
        if(currentSpeed > 0.4f){
            currentSpeed = 0.03;
            currentMultiplier = 1;
        }
        return true;
    }
    return false;
}
inline void spawn(std::vector<Agent>& Agents, bool currentBool, bool isSeeking, int health, float agentSize, std::shared_ptr<sf::Shape> agentShape, sf::Color agentColor, const sf::Vector2f& initialPosition, float movementSpeed){
    Agents.emplace_back(
            currentBool,
            isSeeking,                            //Is affected by current or not
            health,                                 //Agent Health
            agentSize,                               // Agent Size
            agentShape,       // Use the provided shape
            agentColor,       // Agent color
            initialPosition,  // Initial position
            movementSpeed     // Movement speed
        );
}
