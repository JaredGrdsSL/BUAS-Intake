#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


/* --- Json --- */
using json = nlohmann::json;

bool inputtingText = false;

void SaveLevelToJson(std::string levelName = "testLevel") {
	std::cout << "Saving " + levelName + " to json...\n";

	json j;

	auto now = std::chrono::system_clock::now();

	j["test"] = std::format("{:%Y-%m-%d %H:%M:%S}", now);

	std::ofstream output_file("../levels/" + levelName + ".json");
	output_file << j.dump(1);
	output_file.close();


	std::cout << "Saved\n";
}

/* --- Methods --- */
bool IsMouseInRectangle(sf::RectangleShape rect) {
	std::cout << "aoiwsuht" << std::endl;

	sf::Vector2i mousePos = sf::Mouse::getPosition();

	sf::Vector2f rectPos = rect.getPosition();
	sf::Vector2f rectSize = rect.getSize();

	return
		mousePos.x >= rectPos.x &&
		mousePos.x <= rectPos.x + rectSize.x &&
		mousePos.y >= rectPos.y &&
		mousePos.y <= rectPos.y + rectSize.y;
}


/* --- Main --- */
sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Level Builder");

sf::RectangleShape rectangles[99];

int main() {
	window.setFramerateLimit(60);

	int rectanglesArrayLength = sizeof(rectangles) / sizeof(rectangles[0]);
	int nextAvailableRect = 0;
	sf::Vector2f rectDrawStartPos = { 0.f, 0.f }; // The top-left corner of the rectangle you're currently drawing

	int cluh = 0;

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
		}

		sf::RectangleShape* nextRect = &rectangles[nextAvailableRect];

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			bool isMouseInNextRect = false;

			for (sf::RectangleShape rect : rectangles) {
				if (IsMouseInRectangle(rect)) {
					bool isMouseInNextRect = true;
					std::cout << "twin" << std::endl;
				}
			}

			// If this frame's LMB value is different than last frame's, execute. (onMousePressed type logic)
			if (!pMousePressed) {
				if (isMouseInNextRect) {
					std::cout << "mouse in current rect" << std::endl;
					break;
				} else {
					rectDrawStartPos = (sf::Vector2f)sf::Mouse::getPosition(window);

					nextRect->setPosition(rectDrawStartPos);
					nextRect->setFillColor(sf::Color(0, 0, 0, 0));
					nextRect->setOutlineThickness(10);
					nextRect->setOutlineColor(sf::Color(255, 255, 255, 128));
				}
			}
			// LMB is being held here

			if (!isMouseInNextRect) {
				nextRect->setSize((sf::Vector2f)sf::Mouse::getPosition(window) - rectDrawStartPos);
			}

		} else if (pMousePressed) {	// If LMB released
			nextAvailableRect = (nextAvailableRect + 1) % rectanglesArrayLength;
			nextRect->setOutlineColor(sf::Color(255, 255, 255, 255));
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

		for (sf::RectangleShape rect : rectangles) { // Draw all the rectangles
			window.draw(rect);
		}

		if (inputtingText) {	// Draw text input only when inputting text
			window.draw(userTextInput);
		}

		window.display();
	}
}