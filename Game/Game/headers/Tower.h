#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

class Tower {
private:
    sf::RectangleShape shape;

    sf::Vector2f squorbPos;

    float t;

public:
    Tower(float radius = 50.f, sf::Vector2f position = { 0.f, 0.f }, sf::Color color = sf::Color::Black) {
        shape.setSize({ radius,radius });
        shape.setOrigin({ radius, radius }); // Sets origin to center for easier rotation/positioning
        shape.setPosition(position);
        shape.setFillColor(color);
        squorbPos = position;
        t = 0;
    }

    sf::Vector2f getPosition() const {
        return shape.getPosition();
    }

    void setColor(sf::Color color) {
        shape.setFillColor(color);
    }

    // Allows window.draw(mySquorb)
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

