#ifndef AGENT_HPP
#define AGENT_HPP

#include <SFML/Graphics.hpp>
#include <memory> // For smart pointers

class Agent {
private:                                      // Unique ID for the agent
    sf::Color color;                              // Agent's color
    sf::Color trueColor;                           // Agent's true color
    bool colorChanged = false;                            //Records if true color is not the current color
    bool seeking = false;
    float colorChangeDuration = 0.f;              //time in seconds that change should occur for
    sf::Vector2f position;                        // Current position of the agent
    std::shared_ptr<sf::Shape> shape;             // Shape of the agent (polymorphic)
    float size;
    float speed;                                  // Movement speed
    //int health;
    sf::Clock colorClock;                              //to track duration
    bool affectedByCurrent;
    float speedmax = 0.02;
public:
    // Constructor
    Agent(bool affectedByCurrent, bool seeking, int health,float size, std::shared_ptr<sf::Shape> agentShape, sf::Color agentColor, const sf::Vector2f &initialPosition, float movementSpeed);
    int health;
    bool wpressed = false;
    bool spressed = false;
    bool apressed = false;
    bool dpressed = false;
    bool killMe = false;
    sf::Vector2f mySpeedVec = sf::Vector2f(0,0);

   // void NewFunction(const sf::Vector2f &initialPosition);

    void update(sf::Vector2f& currentDirection, float& currentSpeed, std::vector<Agent>& agents,sf::RenderWindow& window,sf::Clock globalClock, float UIdepth, std::vector<int>& killVecCTS);
    void move(const sf::Vector2f& direction, float windX, float windY, float UIdepth, float currentSpeed, std::vector<Agent>& agents, sf::RenderWindow& window, std::vector<int>& killVecCTS);
    void fall(const sf::Vector2f& direction, float current, std::vector<Agent>& agents, sf::RenderWindow& window, std::vector<int>& killVecCTS);

    // Accessors
    int getID() const;
    sf::Color getColor() const;
    sf::Vector2f getPosition() const;
    bool isTouching(Agent);
    bool canSee(Agent& suspect, std::vector<Agent>& agents);
    // Mutators
    void setColor(const sf::Color& newColor);
    void affectColor(const sf::Color& newColor, float duration);
    void setPosition(const sf::Vector2f& newPosition);
    void setSize(float size);
    //void spawn(std::vector<Agent>& Agents, bool currentBool, int health, float agentSize, std::shared_ptr<sf::Shape> agentShape, sf::Color agentColor, const sf::Vector2f& initialPosition, float movementSpeed);
    // Movement
    bool CollisionManagement(std::vector<Agent>& agents, sf::RenderWindow& window, float currentSpeed, std::vector<int>& killVecCTS);
    // Draw the agent
    void draw(sf::RenderWindow& window) const;
};

#endif // AGENT_HPP