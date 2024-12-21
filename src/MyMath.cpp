#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

inline float distance(const sf::Vector2f& point1, const sf::Vector2f& point2) {
    float dx = point2.x - point1.x;
    float dy = point2.y - point1.y;
    return std::sqrt(dx * dx + dy * dy);
}


inline double calculateAngle(const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2f& C) {
    // Vectors AB and BC
    sf::Vector2f AB = {B.x - A.x, B.y - A.y};
    sf::Vector2f BC = {C.x - B.x, C.y - B.y};
    
    // Dot product
    double dotProduct = (AB.x * BC.x) + (AB.y * BC.y);
    
    // Magnitudes of AB and BC
    double magnitudeAB = std::sqrt(AB.x * AB.x + AB.y * AB.y);
    double magnitudeBC = std::sqrt(BC.x * BC.x + BC.y * BC.y);
    
    // Calculate cosine of the angle
    double cosTheta = dotProduct / (magnitudeAB * magnitudeBC);
    
    // Calculate the angle in radians
    return std::acos(cosTheta);
}

inline float pointToSegmentDistance(const sf::Vector2f& point, const sf::Vector2f& segStart, const sf::Vector2f& segEnd) {
    sf::Vector2f segment = segEnd - segStart;
    sf::Vector2f toPoint = point - segStart;
    float segmentLengthSquared = segment.x * segment.x + segment.y * segment.y;

    if (segmentLengthSquared == 0.0f) // Line segment is a single point
        return distance(point, segStart)*distance(point, segStart);

    // Project the point onto the line segment (clamped)
    float t = std::max(0.0f, std::min(1.0f, (toPoint.x * segment.x + toPoint.y * segment.y) / segmentLengthSquared));
    sf::Vector2f projection = segStart + t * segment;
    return distance(point, projection);
}

sf::Vector2f getPolygonCenter(const std::shared_ptr<sf::ConvexShape>& polygon) {
    sf::Vector2f center(0.f, 0.f);
    int pointCount = polygon->getPointCount();
    for (int i = 0; i < pointCount; ++i) {
        center += polygon->getPosition() + polygon->getPoint(i);
    }
    return center / static_cast<float>(pointCount);
}

bool lineIntersectsSegment(const sf::Vector2f& p1, const sf::Vector2f& p2,
                           const sf::Vector2f& q1, const sf::Vector2f& q2) {
    // Calculate directional vectors
    sf::Vector2f r = p2 - p1;
    sf::Vector2f s = q2 - q1;

    // Compute the determinant
    float rxs = r.x * s.y - r.y * s.x;

    // If rxs is zero, lines are parallel or collinear
    if (std::abs(rxs) < 1e-6) {
        return false;
    }

    // Compute t and u
    sf::Vector2f qp = q1 - p1;
    float t = (qp.x * s.y - qp.y * s.x) / rxs;
    float u = (qp.x * r.y - qp.y * r.x) / rxs;

    // Lines intersect if t and u are within [0, 1]
    //std::cout << "lineIntersectsSegment is returning "<< (t >= 0 && t <= 1 && u >= 0 && u <= 1) << std::endl;
    return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

bool lineIntersectsRect(const sf::Vector2f& lineStart, const sf::Vector2f& lineEnd, const sf::FloatRect& rect) {
    // Get the corners of the rectangle
    sf::Vector2f topLeft(rect.left, rect.top);
    sf::Vector2f topRight(rect.left + rect.width, rect.top);
    sf::Vector2f bottomLeft(rect.left, rect.top + rect.height);
    sf::Vector2f bottomRight(rect.left + rect.width, rect.top + rect.height);

    // Check if the line intersects any of the rectangle's edges
    return lineIntersectsSegment(lineStart, lineEnd, topLeft, topRight) ||
           lineIntersectsSegment(lineStart, lineEnd, topRight, bottomRight) ||
           lineIntersectsSegment(lineStart, lineEnd, bottomRight, bottomLeft) ||
           lineIntersectsSegment(lineStart, lineEnd, bottomLeft, topLeft);
}

bool intersectsLine(const std::shared_ptr<sf::Shape>& shape, const sf::Vector2f& lineStart, const sf::Vector2f& lineDir, float lineLength) {
    if (auto circle = std::dynamic_pointer_cast<sf::CircleShape>(shape)) {
        //std::cout << "Got in circ loop of intersectsLine " << std::endl;

        sf::Vector2f circleCenter = circle->getPosition() + sf::Vector2f(circle->getRadius(), circle->getRadius());
        return pointToSegmentDistance(circleCenter, lineStart, lineStart + lineDir * lineLength) <= circle->getRadius();
    } else if (auto rect = std::dynamic_pointer_cast<sf::RectangleShape>(shape)) {
        //std::cout << "Got in rect loop of intersectsLine " << std::endl;
        sf::FloatRect bounds = rect->getGlobalBounds();
        return lineIntersectsRect(lineStart, lineStart + lineDir * lineLength, bounds);
    } else if (auto polygon = std::dynamic_pointer_cast<sf::ConvexShape>(shape)) {
        //std::cout << "Got in poly loop of intersectsLine " << std::endl;
        int pointCount = polygon->getPointCount();
        for (int i = 0; i < pointCount; ++i) {
            sf::Vector2f point1 = polygon->getPosition() + polygon->getPoint(i);
            sf::Vector2f point2 = polygon->getPosition() + polygon->getPoint((i + 1) % pointCount);
            if (lineIntersectsSegment(lineStart, lineStart + lineDir * lineLength, point1, point2)) {
                return true;
            }
        }
    }
    return false;
}