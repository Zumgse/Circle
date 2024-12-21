#include "Agent.hpp"
#include <cmath>
#include <iostream>
#include "MyMath.cpp"
#include "GameOver.cpp"
#include <random>
// Constructor
inline Agent::Agent(bool currentBool, bool consSeeking, int agentHealth, float agentSize, std::shared_ptr<sf::Shape> agentShape, sf::Color agentColor, const sf::Vector2f& initialPosition, float movementSpeed)
    : size(agentSize), color(agentColor), position(initialPosition), shape(agentShape), speed(movementSpeed){
    trueColor = color;
    health = agentHealth;
    seeking = consSeeking;
    affectedByCurrent = currentBool;
    shape->setFillColor(color);
    shape->setPosition(initialPosition);
    //shape->setScale(sf::Vector2f(1,1)*size);


    //Sizing shapes differently:
     if (auto circle = std::dynamic_pointer_cast<sf::CircleShape>(shape)) {
        circle->setRadius(health*10);  // For circles
    } else if (auto rectangle = std::dynamic_pointer_cast<sf::RectangleShape>(shape)) {
        rectangle->setSize(sf::Vector2f(size, size));  // For rectangles (square size)
    } else if (auto triangle = std::dynamic_pointer_cast<sf::ConvexShape>(shape)) {
        // Assuming triangle has 3 points (scaling to maintain shape)
        triangle->setPointCount(3);  
        // Set the size of the triangle (scaling based on one side or bounding box)
        // Set points of the triangle
        triangle->setPoint(0, sf::Vector2f(0.f, -size/2));   // Top point
        triangle->setPoint(1, sf::Vector2f(size/2, size/2));   // Bottom right point
        triangle->setPoint(2, sf::Vector2f(-size/2, size/2));  // Bottom left point
    } else if (auto hexagon = std::dynamic_pointer_cast<sf::ConvexShape>(shape)) {
        hexagon->setPointCount(6);  // Hexagon has 6 points
        // Define points of the hexagon based on size
        const float angle = 3.14159f / 3;  // 60 degrees
        for (int i = 0; i < 6; ++i) {
            float x = size * cos(i * angle);
            float y = size * sin(i * angle);
            hexagon->setPoint(i, sf::Vector2f(x, y));
        }
    }
}

inline void Agent::update(sf::Vector2f& fallDirection, float& currentSpeed, std::vector<Agent>& agents,sf::RenderWindow& window, sf::Clock globalClock, float UIdepth,std::vector<int>& killVecCTS) {
    if (colorChanged && colorClock.getElapsedTime().asSeconds() >= colorChangeDuration) {
        // After the duration ends, reset the color to white or the original color
        if (health > 0){
            health -= 1;
            if(health <= 0){
                killMe = true;
            }
            //std::cout<< "myHealth is " << health << std::endl;
            //std::cout<< "playerHealth is " << agents[0].health << std::endl;
            speedmax = 0.01 + 0.01*(15/health);
            if(speedmax < 0.01){speedmax = 0.01;}
            //std::cout<< "speedmax is " << speedmax << std::endl;
            if (auto circle = std::dynamic_pointer_cast<sf::CircleShape>(shape)) {
                if(health <= 0){
                    killVecCTS[0] +=1;
                    //std::cout << "circskilled is" << killVecCTS[0] << std::endl;
                }
            circle->setRadius(health*10);  // For circles
            } 
        }
        else{ //this case is for if dead
            //std::cout<< "in death loop " << std::endl;
            if (auto circle = std::dynamic_pointer_cast<sf::CircleShape>(shape)) {
            circle->setRadius(health*10);  // For circles
            killMe = true;
            setPosition(sf::Vector2f(-100,-100));
            //std::cout << "circskilled is" << killVecCTS[0] << std::endl;
            killVecCTS[0] +=1;
            return;
            }
            
        }
        shape->setFillColor(trueColor);  // Example reset to white
        colorChanged = false;  // Reset the flag
    }
    
    if(affectedByCurrent == true){
        //std::cout << "attempting to fall "<<std::endl;
        fall(fallDirection, currentSpeed, agents, window, killVecCTS);
    }
    else{
        if (wpressed || spressed || apressed || dpressed) {
                if(spressed){
                    mySpeedVec.y = mySpeedVec.y + speedmax/100;
                    if (mySpeedVec.y > speedmax){mySpeedVec.y = speedmax;}
                    //window.setSize(sf::Vector2u(900,450));
                    //currentSpeed = 0.4;
                    //std::cout << "mySpeedVec y is " << mySpeedVec.y << std:: endl;
                    spressed = false;
                }
                if(wpressed){
                    mySpeedVec.y = mySpeedVec.y - speedmax/100;
                    if (mySpeedVec.y < -speedmax){mySpeedVec.y = -speedmax;}
                    //std::cout << "windX is " << window.getSize().x << std::endl;
                    wpressed = false;
                }
                if(apressed){
                    mySpeedVec.x = mySpeedVec.x - speedmax/100;
                    if (mySpeedVec.x < -speedmax){mySpeedVec.x = -speedmax;}
                    //std::cout << "mySpeedVec x is " << mySpeedVec.x << std:: endl;
                    apressed = false;
                }
                if(dpressed){
                    mySpeedVec.x = mySpeedVec.x + speedmax/100;
                    if (mySpeedVec.x > speedmax){mySpeedVec.x = speedmax;}
                    //std::cout << "mySpeedVec x is " << mySpeedVec.x << std:: endl;
                    dpressed = false;
                }
            
            }
            move(mySpeedVec, window.getSize().x, window.getSize().y, UIdepth, currentSpeed, agents, window, killVecCTS); // Move the first agent
            //std::cout << "after move" << std::endl;
    }
    //Seeker behavior:
    if(seeking == true){
        if(canSee(agents[0], agents) == true){
            sf::Vector2f currentCenter;
            if (auto currentCircle = std::dynamic_pointer_cast<sf::CircleShape>(shape)) {
                currentCenter = currentCircle->getPosition() + sf::Vector2f(currentCircle->getRadius(), currentCircle->getRadius());
            }
            sf::CircleShape* playerCircle = dynamic_cast<sf::CircleShape*>(agents[0].shape.get());

            sf::Vector2f targetCenter = agents[0].getPosition() + sf::Vector2f(playerCircle->getRadius(), playerCircle->getRadius());

            sf::Vector2f direction = targetCenter - currentCenter;
            float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            sf::Vector2f normalizedDirection = direction / (50.f * magnitude);
    
            if(window.getSize().x>1000){
                normalizedDirection = normalizedDirection * currentSpeed * 15.f;
            }
            else{
                normalizedDirection = normalizedDirection * currentSpeed * 40.f;
            }
            move(normalizedDirection, window.getSize().x, window.getSize().y, UIdepth, currentSpeed ,agents, window, killVecCTS);
            //std::cout << " After seekerMove " << std::endl;
        }
        else{
            //std::cout << " WHERE ARE YOU " << std::endl;
        }
    }
}

// Accessors
inline sf::Color Agent::getColor() const {
    return color;
}

inline sf::Vector2f Agent::getPosition() const {
    return position;
}

// Mutators
inline void Agent::setColor(const sf::Color& newColor) {
    color = newColor;
    shape->setFillColor(color);
}
inline void Agent::affectColor(const sf::Color& newColor, float duration) {
    color = newColor;
    shape->setFillColor(color);
    colorChangeDuration = duration;
    colorClock.restart();  // Start the clock for this color change duration
    colorChanged = true;  // Indicate that the color has been changed
}

inline void Agent::setPosition(const sf::Vector2f& newPosition) {
    position = newPosition;
    shape->setPosition(position);
}

inline void Agent::setSize(float size2){
    size = size2;
    shape->setScale(sf::Vector2f(1,1)*size);
}

inline bool Agent::canSee(Agent& suspect, std::vector<Agent>& agents) {
    // Get the center of the current agent
    sf::Vector2f currentCenter;
    if (auto currentCircle = std::dynamic_pointer_cast<sf::CircleShape>(shape)) {
        currentCenter = currentCircle->getPosition() + sf::Vector2f(currentCircle->getRadius(), currentCircle->getRadius());
    } else if (auto currentRect = std::dynamic_pointer_cast<sf::RectangleShape>(shape)) {
        currentCenter = currentRect->getPosition() + (currentRect->getSize() / 2.0f);
    } else if (auto currentPolygon = std::dynamic_pointer_cast<sf::ConvexShape>(shape)) {
        currentCenter = getPolygonCenter(currentPolygon); // A helper function to calculate the polygon's center
    } else {
        return false; // Unsupported shape type
    }

    // Get the center of the suspect
    sf::Vector2f suspectCenter;
    if (auto suspectCircle = std::dynamic_pointer_cast<sf::CircleShape>(suspect.shape)) {
        suspectCenter = suspectCircle->getPosition() + sf::Vector2f(suspectCircle->getRadius(), suspectCircle->getRadius());
        //std::cout << "Identified as circle "<< std::endl;
    } else if (auto suspectRect = std::dynamic_pointer_cast<sf::RectangleShape>(suspect.shape)) {
        suspectCenter = suspectRect->getPosition() + (suspectRect->getSize() / 2.0f);
    } else if (auto suspectPolygon = std::dynamic_pointer_cast<sf::ConvexShape>(suspect.shape)) {
        suspectCenter = getPolygonCenter(suspectPolygon); // Helper function for polygon center
    } else {
        return false; // Unsupported shape type
    }

    // Check for obstacles between the two points
    sf::Vector2f direction = suspectCenter - currentCenter;
    float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f normalizedDirection = direction / magnitude;

    for (const auto& otherAgent : agents) {
        // Skip the current agent and the suspect
        //std::cout << "Address of this: " << this << ", Address of otherAgent: " << &otherAgent << std::endl;
        //std::cout << "Address of suspect: " << &suspect << ", Address of otherAgent: " << &otherAgent << std::endl;
        if (this == &otherAgent || &suspect == &otherAgent) {
            
            //std::cout << "skip sometimes works "<< std::endl;

            continue;
        }

        // Check if this agent's shape intersects the line between currentCenter and suspectCenter
        if (auto otherShape = otherAgent.shape) {
            //std::cout << "magnitude is " << magnitude << std::endl;
            if (intersectsLine(otherShape, currentCenter, normalizedDirection, magnitude)) {
                //std::cout << "intersectsLine is true "<< std::endl;
                return false; // Line of sight is blocked
            }
        }
    }

    return true; // No obstacles, the current agent can see the suspect
}

inline bool Agent::isTouching(Agent suspect) {

    // Check if the current agent's shape is a circle.
    if (auto circle = std::dynamic_pointer_cast<sf::CircleShape>(shape)) {
        
        // Check if the suspect is a convex polygon.
        if (auto polygon = std::dynamic_pointer_cast<sf::ConvexShape>(suspect.shape)) {
            // Get the center of the circle.
            sf::Vector2f circleCenter = circle->getPosition() + sf::Vector2f(circle->getRadius(), circle->getRadius());
            float circleRadius = circle->getRadius();
            // Iterate through all the points of the polygon.
            std::vector<float> distances = {1000,1000,1000}; //Distances of each point to the circle
            int polyPointNum = polygon->getPointCount();
            distances.resize(polyPointNum);
            bool isBelowThreshhold = false; // Describes if one point on the polygon is within 2sqrt(r^2+(s/2)^2), endangering the circle going BETWEEN two points

            for (unsigned int i = 0; i < polygon->getPointCount(); ++i) {
                sf::Vector2f polygonPos = polygon->getPosition();
                sf::Vector2f polygonPoint = polygonPos + polygon->getPoint(i);
                // Calculate the distance between the center of the circle and the point of the polygon.
                float dx = polygonPoint.x - circleCenter.x;
                float dy = polygonPoint.y - circleCenter.y;
                float distance = std::sqrt(dx * dx + dy * dy);
               
                distances[i] = distance;
               
                // If the distance is less than or equal to the circle's radius, they are touching.
                if (distance <= circleRadius) {
                    return true; // The circle is touching the polygon.
                }

                //std::cout<< "circ to segment distance is " << pointToSegmentDistance(circleCenter, polygonPos + polygon->getPoint(i), polygonPos + polygon->getPoint((i+1) % polyPointNum)) << std::endl;

                //If the distance between the circle center and a segment is less than the circleRadius, they are touching.
                if (pointToSegmentDistance(circleCenter, polygonPos + polygon->getPoint(i), polygonPos + polygon->getPoint((i+1) % polyPointNum)) < circleRadius){
                    return true;
                }
                else if (pointToSegmentDistance(circleCenter, polygonPos + polygon->getPoint(i), polygonPos + polygon->getPoint((i-1) % polyPointNum)) < circleRadius){
                    return true;
                }
               
            }
        }
        if (auto rect = std::dynamic_pointer_cast<sf::RectangleShape>(suspect.shape)) {
            sf::Vector2f circleCenter = circle->getPosition() + sf::Vector2f(circle->getRadius(), circle->getRadius());
            float circleRadius = circle->getRadius();
            sf::FloatRect rectBounds = rect->getGlobalBounds();
            //std::cout << " before boundscheck " << std::endl;
            if(circleCenter.x + circleRadius > (rect->getGlobalBounds()).left && 
            circleCenter.x - circleRadius < (rect->getGlobalBounds()).left + rectBounds.width &&
            circleCenter.y + circleRadius > rectBounds.top &&
            circleCenter.y - circleRadius < rectBounds.top + rectBounds.height)
            {
                //std::cout << " after boundscheck " << std::endl;
                return true;
            }
            return false;
        }
        if (auto otherCirc = std::dynamic_pointer_cast<sf::CircleShape>(suspect.shape)) {
        sf::Vector2f circleCenter = circle->getPosition() + sf::Vector2f(circle->getRadius(), circle->getRadius());
        float circleRadius = circle->getRadius();
        sf::Vector2f suspectCenter = otherCirc->getPosition() + sf::Vector2f(otherCirc->getRadius(), otherCirc->getRadius());
        float suspectRadius = otherCirc->getRadius();
        if(distance(circleCenter, suspectCenter) <= circleRadius + suspectRadius){
            return true;
        }
        return false;

        }
        return false;
    }
    // Check if the current agent's shape is a polygon.
    if (auto polygon = std::dynamic_pointer_cast<sf::ConvexShape>(shape)) {
        
        // Check if the suspect is a circle.
        if (auto circle = std::dynamic_pointer_cast<sf::CircleShape>(suspect.shape)) {
            // Get the center of the circle.
            sf::Vector2f circleCenter = circle->getPosition() + sf::Vector2f(circle->getRadius(), circle->getRadius());
            float circleRadius = circle->getRadius();
            // Iterate through all the points of the polygon.
            std::vector<float> distances = {1000,1000,1000}; //Distances of each point to the circle
            int polyPointNum = polygon->getPointCount();
            distances.resize(polyPointNum);
            bool isBelowThreshhold = false; // Describes if one point on the polygon is within 2sqrt(r^2+(s/2)^2), endangering the circle going BETWEEN two points

            for (unsigned int i = 0; i < polygon->getPointCount(); ++i) {
                sf::Vector2f polygonPos = polygon->getPosition();
                sf::Vector2f polygonPoint = polygonPos + polygon->getPoint(i);
                // Calculate the distance between the center of the circle and the point of the polygon.
                float dx = polygonPoint.x - circleCenter.x;
                float dy = polygonPoint.y - circleCenter.y;
                float distance = std::sqrt(dx * dx + dy * dy);
               
                distances[i] = distance;
               
                // If the distance is less than or equal to the circle's radius, they are touching.
                if (distance <= circleRadius) {
                    return true; // The circle is touching the polygon.
                }

                //std::cout<< "circ to segment distance is " << pointToSegmentDistance(circleCenter, polygonPos + polygon->getPoint(i), polygonPos + polygon->getPoint((i+1) % polyPointNum)) << std::endl;

                //If the distance between the circle center and a segment is less than the circleRadius, they are touching.
                if (pointToSegmentDistance(circleCenter, polygonPos + polygon->getPoint(i), polygonPos + polygon->getPoint((i+1) % polyPointNum)) < circleRadius){
                    return true;
                }
                else if (pointToSegmentDistance(circleCenter, polygonPos + polygon->getPoint(i), polygonPos + polygon->getPoint((i-1) % polyPointNum)) < circleRadius){
                    return true;
                }
               
            }
        }
        return false;
    }
    return false;
}
    // Add checks for other shape types (e.g., rectangle, triangle, etc.) as needed.
inline bool Agent::CollisionManagement(std::vector<Agent>& agents, sf::RenderWindow& window, float currentSpeed, std::vector<int>& killVecCTS) {
    // Iterate over agents with an iterator for safe modification
    for (auto it = agents.begin(); it != agents.end(); ) {
        // Ensure the agent is not interacting with itself
        if (this != &(*it)) {
            // Check if the current agent is touching the other agent
            if (this->isTouching(*it)) {
                // Handle interaction if the other agent's shape is a rectangle
                if (auto rect = std::dynamic_pointer_cast<sf::RectangleShape>((*it).shape)) {
                    //std::cout << "inside collision rect section" << std::endl;
                    health += 1; // Increment health
                    speedmax = 0.01 + 0.01*(15/health);
                    // Safely erase the other agent and continue iteration
                    killVecCTS[2] +=1;
                    it = agents.erase(it);
                    //std::cout << "after erasure" << std::endl;
                    // If this agent's shape is a circle, update its radius
                    if (auto circle = std::dynamic_pointer_cast<sf::CircleShape>(shape)) {
                        circle->setRadius(health * 10); // Update radius based on health
                        //std::cout << "after radiusSet" << std::endl;
                    }

                    // If health reaches 1, handle game-over logic


                    return true; // Collision handled, exit function
                }
                if (health == 1) {
                        health = 0;
                        if (this == &agents.front()) {
                            showGameOver(window, killVecCTS); // Display game-over screen
                        }
                        else{
                            
                        }
                    }
                else{
                    if (!colorChanged && this->affectedByCurrent == false) {
                        if((*it).affectedByCurrent == false && (*it).colorChanged == false){ //affect other agent if is not affected by current
                           // std::cout << " in the affected zone " << std::endl;
                            (*it).colorChanged = true;
                            (*it).affectColor(sf::Color::White, 1);
                            affectColor(sf::Color::White, 1);
                            //std::cout << " past the affected zone " << std::endl;
                        }
                        else if((*it).affectedByCurrent == true){ //reduce other agent's health if non-chaser/player
                            affectColor(sf::Color::White, 1);
                            (*it).health -=1;
                        }
                        //std::cout << " other health is " << (*it).health << std::endl;
                        if ((*it).health <= 0 && (*it).affectedByCurrent == true){
                            affectColor(sf::Color::White, 1);
                            killVecCTS[1] +=1;
                            it = agents.erase(it);
                        }
                    } else if (&(*it) == &agents.front() && (*it).colorChanged == false) {
                        (*it).affectColor(sf::Color::White, 1);
                    }
                    return true;
                }
            } else {
                // Move to the next agent if no collision
                ++it;
            }
        } else {
            // Move to the next agent if it's the same agent
            ++it;
        }
    }
    return false; // No collision occurred
}

// Movement
inline void Agent::move(const sf::Vector2f& direction, float windX, float windY, float UIdepth, float currentSpeed, std::vector<Agent>& agents, sf::RenderWindow& window, std::vector<int>& killVecCTS) {
    sf::Vector2f oldpos = position;
    position += (direction);
    windX = window.getSize().x;
    windY = window.getSize().y;
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.x > windX - shape->getLocalBounds().width)
        position.x = windX - shape->getLocalBounds().width;
    if (position.y > windY - UIdepth - shape->getLocalBounds().height)
        position.y = windY - UIdepth - shape->getLocalBounds().height;
    shape->setPosition(position);
    if (CollisionManagement(agents, window, currentSpeed, killVecCTS)){
        //std::cout << "after colMan" << std::endl;
        position = oldpos;
        //std::cout << "after posSet" << std::endl;
    }
}

inline void Agent::fall(const sf::Vector2f& direction, float current, std::vector<Agent>& agents, sf::RenderWindow& window, std::vector<int>& killVecCTS) {
    //std::cout << "falling" << std::endl;
    sf::Vector2f oldpos = position;
    position += direction * current;
    if (CollisionManagement(agents, window, current, killVecCTS)){
        position = oldpos;
    }
    shape->setPosition(position);
    
    //if (CollisionManagement(agents, window)){
    //    position = oldpos;
    //}
}

// Draw the agent
inline void Agent::draw(sf::RenderWindow& window) const {
    window.draw(*shape);
}