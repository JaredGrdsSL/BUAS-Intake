#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

class Bullet {
private:
    sf::CircleShape shape;

    sf::Vector2f bulletPos;
    sf::Vector2f _direction;

    float speed = 10000.f;

    float t;

public:
    Bullet(sf::Vector2f position = { 0.f, 0.f }, sf::Vector2f direction = {0,0}, std::vector<Squorb> squorbs = {}) {
        shape.setRadius(10.f);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::White);

        bulletPos = position;
        _direction = direction;
        t = 0;
    }

    void update(float delta, sf::RenderWindow& window) {
        bulletPos += _direction * speed * delta;
        //std::cout << std::to_string(_direction.x) +", " + std::to_string(_direction.y) << std::endl;

        sf::Vector2f globalBulletPos = bulletPos - (sf::Vector2f)window.getPosition();
        setPosition(globalBulletPos);
    }

    sf::Vector2f getPosition() const {
        return shape.getPosition();
    }

    void setPosition(sf::Vector2f pos) {
        shape.setPosition(pos);
    }

    void setColor(sf::Color color) {
        shape.setFillColor(color);
    }

    // Allows window.draw(mySquorb)
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

