#pragma once

#include <SFML/Graphics.hpp>
#include <headers/Bullet.h>

class Tower {
private:
	sf::RectangleShape shape;

	sf::Vector2f towerPos;

	std::vector<Squorb> _squorbs;

	std::vector<Bullet> bullets;

	float t;
	float pT;

	sf::Vector2f bulletDirections[4] = {	// List of up/down/left/right offsets so we can check the tiles around currentPos
	{0, -1},
	{1, 0},
	{0, 1},
	{-1, 0}
	};

public:
	Tower(float size = 50.f, sf::Vector2f position = { 0.f, 0.f }, sf::Color color = sf::Color::Yellow, std::vector<Squorb> squorbs = {}) {
		shape.setSize({ size,size });
		shape.setOrigin({ size, size }); // Sets origin to center for easier rotation/positioning
		shape.setPosition(position);
		shape.setFillColor(color);

		_squorbs = squorbs;
		towerPos = position;
		t = 0;
		pT = 0;
	}

	void update(float delta, sf::RenderWindow& window) {
		t += delta;

		sf::Vector2f globalTowerPos = towerPos - (sf::Vector2f)window.getPosition();
		setPosition(globalTowerPos);

		// Shoot a bullet every few seconds
		if ((int)std::floorf(t) % 2 == 0 && (int)std::floorf(t) != pT) {
			Bullet bullet(towerPos, bulletDirections[(int)std::floorf(t) % 4], _squorbs);
			bullets.push_back(bullet);
		}

		for (Bullet bullet : bullets) {
			bullet.update(delta, window);
			bullet.draw(window);
		}
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

