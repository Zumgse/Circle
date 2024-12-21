#include <SFML/Graphics.hpp>
#include <cmath>
//TO MAKE THE .O: g++ -Isrc/include -c main.cpp
//TO MAKE THE .EXE: g++ main.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system
//./main.exe

class Tri{
    private:
        sf::ConvexShape triangle;
        float size; // length of a triangle side
         // Helper function to calculate the vertices of the triangle
        void calculateVertices() {
            // Center of the triangle is the position
            sf::Vector2f position = triangle.getPosition();

            // Calculate the three vertices of the triangle
            float height = std::sqrt(3) / 2 * size; // Height of the equilateral triangle

            // Set points relative to the center
            triangle.setPoint(0, sf::Vector2f(position.x, position.y - 2 * height / 3));  // Top vertex
            triangle.setPoint(1, sf::Vector2f(position.x - size / 2, position.y + height / 3)); // Bottom-left vertex
            triangle.setPoint(2, sf::Vector2f(position.x + size / 2, position.y + height / 3)); // Bottom-right vertex
        }
    public:
    // Constructor
        Tri(float size, sf::Vector2f position, float rotation = 0) : size(size) {
            triangle.setPointCount(3); // 3 points for a triangle
            triangle.setFillColor(sf::Color::Blue); // Default fill color
            triangle.setPosition(position); // Set position
            triangle.setRotation(rotation); // Set rotation
            calculateVertices(); // Calculate the vertices based on size and position
    }
    //Settors:
        void setSize(float newSize) {
        size = newSize;
        calculateVertices();
    }

    void setPosition(const sf::Vector2f& position) {
        triangle.setPosition(position);
        calculateVertices();
    }

    void setRotation(float angle) {
        triangle.setRotation(angle);
    }

    void setFillColor(const sf::Color& color) {
        triangle.setFillColor(color);
    }

    void draw(sf::RenderWindow& window) {
    window.draw(triangle);
}
};