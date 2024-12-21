#include <SFML/Graphics.hpp>
//#include "Agent.cpp"
#include <cmath>
#include <iostream>
#include "spawnArbiter.cpp"
#include "EntryScreen.cpp"
//#include "scoreArbiter.cpp"
//How to run:
/*g++ -Isrc/include -c src/main.cpp -o main.o
 g++ -Isrc/include -c src/Agent.cpp -o Agent.o
 g++ main.o Agent.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system
 ./main.exe*/

int main()
{
    sf::Clock globalClock;
    float lastUpdateTime = 0.0f; // Keeps track of the last update time
    const float updateInterval = 1.0f / 50.0f; // 50 updates per second (0.02 seconds)
    //Define all Static qualities
    float mvtSpeed = 0.1f;
    int playerHealth = 5;
    float circRadius = 10.f;
    float windX = 600;
    float windY = 300;
    float UIdepth = 100;
    float xmvtLimit = windX - 2 * circRadius; //Limit of where the circle can go so as not to escape the window
    float ymvtLimit = windY - 2 * circRadius;
    float lastSpawnTime = 0;
    float currentSpeed = 0.01;
    float currentMultiplier = 1.08;
    int numTrisKilled = 0;
    int numSqsKilled = 0;
    int numCircsKilled = 0;
    std::vector<int> killVecCTS;
    killVecCTS.push_back(numCircsKilled);
    killVecCTS.push_back(numTrisKilled);
    killVecCTS.push_back(numSqsKilled);

    sf::Vector2f currentDirection(0,0);
    std::random_device rd; // Seed source
    std::mt19937 gen(rd()); // Mersenne Twister engine

    sf::Font font;
    if (!font.loadFromFile("src/Khamenet-8OLn0.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
    }

    sf::RenderWindow window(sf::VideoMode(windX, windY+UIdepth), "Circle v1.0");
    sf::View view = window.getDefaultView();
    showEntryScreen(window);
    
    //Define vector of agents
    std::vector<Agent> Agents;
   /* Agents.emplace_back(
        0,                               // Agent ID
        20.f,                               // Agent Size
        std::make_shared<sf::CircleShape>(30.f), // Circle shape with radius 30
        sf::Color::Red,                   // Agent color
        sf::Vector2f(200.f, 200.f),       // Initial position
        0.2f                              // Movement speed
    );
    Agents.emplace_back(
        1,                               // Agent ID
        100.f,                               // Agent Size
        std::make_shared<sf::ConvexShape>(30.f), // Triangle shape with side length 30
        sf::Color::Yellow,                   // Agent color
        sf::Vector2f(400.f, 400.f),       // Initial position
        0.1f                              // Movement speed
    );
    Agents.emplace_back(
        1,                               // Agent ID
        100.f,                               // Agent Size
        std::make_shared<sf::ConvexShape>(30.f), // Triangle shape with side length 30
        sf::Color::Yellow,                   // Agent color
        sf::Vector2f(100.f, 100.f),       // Initial position
        0.1f                              // Movement speed
    );*/
    spawn(Agents, false, false, playerHealth, 20, std::make_shared<sf::CircleShape>(20), sf::Color::Red, sf::Vector2f(200.f, 200.f), 0.2f);
    //spawn(Agents, false, true, 2, 100.f, std::make_shared<sf::CircleShape>(20), sf::Color::Blue, sf::Vector2f(300.f, 300.f), 0.2f);
    //spawn(Agents, true, 1, 100, std::make_shared<sf::ConvexShape>(30.f), sf::Color::Yellow, sf::Vector2f(500.f, 400.f), 0.2f);
    //spawn(Agents, true, 1, 100, std::make_shared<sf::ConvexShape>(30.f), sf::Color::Yellow, sf::Vector2f(500.f, 100.f), 0.2f);

    while (window.isOpen())
    {
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (event.type == sf::Event::Resized) {
            // Adjust the view to match the new size
            view.setSize(event.size.width, event.size.height);
            
            view.setCenter(event.size.width / 2.0f, event.size.height / 2.0f);

            view.setSize(event.size.width, event.size.height);
            
            window.setView(view);

        }

        sf::Vector2f direction(0.f,0.f); //direction of player

         if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            Agents[0].wpressed = true; // Move up
            //killVecCTS[0] = 30;
        }
        else{ Agents[0].wpressed = false;}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
             Agents[0].apressed = true; // Move left
        }
        else{ Agents[0].apressed = false;}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
             Agents[0].spressed = true; // Move down
        }
        else{ Agents[0].spressed = false;}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
             Agents[0].dpressed = true; // Move right
        }
        else{ Agents[0].dpressed = false;}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            return 0;
        }
        
        window.clear();
        float elapsedTime = globalClock.getElapsedTime().asSeconds();
        int score = scoreArbiter(killVecCTS[0],killVecCTS[1],killVecCTS[2]);

        for (auto it = Agents.begin(); it != Agents.end(); ) {
             //std::cout<< "begin " << std::endl;
            // Check if the agent's position is out of bounds and remove it if necessary
            if (it->getPosition().x < -10) {

                it = Agents.erase(it); 
                continue; 
            }
            
            it->draw(window);
            
            //float elapsedTime = globalClock.getElapsedTime().asSeconds();
            if (elapsedTime - lastUpdateTime >= updateInterval) {
                //int score = scoreArbiter(killVecCTS[0],killVecCTS[1],killVecCTS[2]);
                sf::Vector2f fallDirection(-1.f, 0.f);
                it->update(fallDirection, currentSpeed, Agents, window, globalClock, UIdepth, killVecCTS);
                if (it->killMe == true){
                    
                    it = Agents.erase(it);
                    continue;
                }

                sf::Vector2f direction(0.f, 0.f);
                
            }

            // Move to the next agent
            ++it;
        }
        UIArbiter(
                UIdepth,
                score,
                elapsedTime,
                font,
                window
                );
        spawnArbiter(
            globalClock,
            lastSpawnTime,
            currentSpeed,
            currentMultiplier,
            Agents,
            window,
            UIdepth,
            killVecCTS
        );
        /*if(willSpawn(globalClock, lastSpawnTime, currentSpeed)){

            float windY = window.getSize().y;
            std::uniform_real_distribution<float> disty(0.0f, windY);
            float randomFloat = disty(gen);
            spawn(Agents, true, 3, 100.f, std::make_shared<sf::ConvexShape>(30.f), sf::Color::Yellow, sf::Vector2f(600.f, randomFloat), 0.2f);
        }*/
        //Tri1.draw(window);
        window.display();
    }

    return 0;
}