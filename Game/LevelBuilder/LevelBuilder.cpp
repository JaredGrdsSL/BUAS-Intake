#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


/* --- Json --- */
using json = nlohmann::json;

bool inputtingText = false;

void SaveLevelToJson(std::string levelName = "testLevel") {
	//	std::cout << "Saving " + levelName + " to json...\n";
	//
	//	json j;
	//
	//	auto now = std::chrono::system_clock::now();
	//
	//	j["test"] = std::format("{:%Y-%m-%d %H:%M:%S}", now);
	//
	//	std::ofstream output_file("../levels/" + levelName + ".json");
	//	output_file << j.dump(1);
	//	output_file.close();
	//
	//
	//	std::cout << "Saved\n";
}

int main() {
	sf::RenderWindow window(sf::VideoMode({ 1080, 1080 }), "Level Builder");
	window.setFramerateLimit(60);

	// Setup level variables
	const int levelSize = 10;
	const int tileSize = window.getSize().x / levelSize;

	int tileIDs[levelSize][levelSize];					// The numerical ID of the tile. This is passed to the game
	int selectedTileID;									// The currently selected tile ID for placing
	
	sf::RectangleShape* tiles[levelSize][levelSize];	// The rectangleshape to visualize the grid

	// Default grid color
	sf::Color backgroundColor = sf::Color(50, 50, 50);

	for (int y = 0; y < levelSize; y++) {
		for (int x = 0; x < levelSize; x++) {
			tiles[x][y] = new sf::RectangleShape(sf::Vector2f(tileSize, tileSize));

			tiles[x][y]->setPosition(sf::Vector2f(x * tileSize, y * tileSize));

			tiles[x][y]->setFillColor(backgroundColor);
			tiles[x][y]->setOutlineThickness(1.0f);
			tiles[x][y]->setOutlineColor(sf::Color::Black);
		}
	}

	// The mouse press value on the previous frame
	bool pMousePressed = false;

	// The save key (S) pressed value on the previous frame
	bool pSaveKeyPressed = false;


	// Text
	sf::Font font;
	if (!font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
		std::cout << "Error Loading Font.";
	}

	sf::Text userTextInput(font, "Name: ", 24);
	userTextInput.setPosition({ 10,10 });
	userTextInput.setFillColor(sf::Color::White);

	std::string userInput = "";

	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
				window.close();
			}

			if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
				if (textEvent->unicode == 8) {	// Backspace
					if (!userInput.empty()) userInput.pop_back();
				} else if (textEvent->unicode < 128) {
					userInput += (char)(textEvent->unicode);
				}

				userTextInput.setString("Name: " + userInput);
			}

			if (const auto* keyboardEvent = event->getIf<sf::Event::KeyPressed>()) {
				switch (keyboardEvent->code) {
				case sf::Keyboard::Key::Num1:
					selectedTileID = 1;
				}
			}
		}

		for (int y = 0; y < levelSize; y++) {
			for (int x = 0; x < levelSize; x++) {
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

				// Check if the mouse is in the bounds of the square it's currently checking
				if (tiles[x][y]->getGlobalBounds().contains(mousePos)) {

					tiles[x][y]->setFillColor(sf::Color(128, 128, 128));

					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) { // LMB is being held
						std::cout << std::to_string(x) + ", " + std::to_string(y) << std::endl;
					}
				} else {
					tiles[x][y]->setFillColor(backgroundColor);
				}
			}
		}


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && !pSaveKeyPressed) {
				inputtingText = true;
			}
			pSaveKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) && inputtingText) {
			inputtingText = false;
			std::string userTextInputString = userTextInput.getString();
			userTextInputString.erase(0, 5);
			SaveLevelToJson(userTextInputString);
		}

		pMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

		window.clear();

		for (int x = 0; x < levelSize; x++) {
			for (int y = 0; y < levelSize; y++) {
				window.draw(*tiles[x][y]); // Dereference the pointer to draw the object
			}
		}

		if (inputtingText) {	// Draw text input only when inputting text
			window.draw(userTextInput);
		}

		window.display();
	}
}