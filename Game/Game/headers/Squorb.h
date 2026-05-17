#pragma once

#include <SFML/Graphics.hpp>

class Squorb {
private:
    sf::CircleShape shape;
    std::vector<sf::Vector2i> squorbPath;

    sf::Vector2f squorbPos;

    bool alive;

    float t;

public:
    Squorb(float radius = 50.f, sf::Vector2f position = { 0.f, 0.f }, sf::Color color = sf::Color::Black, std::vector<sf::Vector2i> path = {}) {
        shape.setRadius(radius);
        shape.setOrigin({ radius, radius }); // Sets origin to center for easier rotation/positioning
        shape.setPosition(position);
        shape.setFillColor(color);

        squorbPath = path;
        squorbPos = position;

        alive = true;
        t = 0;
    }

    void walkAlongPath(float delta, sf::Vector2f windowPos, float tileSize) {
        t += delta;

        int pathIndex = (int)std::fmodf(std::floor(t), squorbPath.size());

        squorbPos += (sf::Vector2f)(squorbPath.at(pathIndex)) * tileSize * delta;

        sf::Vector2f globalSquorbPos = squorbPos - windowPos;
        setPosition(globalSquorbPos);
    }

    void setPosition(sf::Vector2f pos) {
        shape.setPosition(pos);
    }

    sf::Vector2f getPosition() const {
        return shape.getPosition();
    }

    void setColor(sf::Color color) {
        shape.setFillColor(color);
    }

    void kill() {
        alive = false;
    }

    bool isAlive() const {
        return alive;
    }

    // Allows window.draw(mySquorb)
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

