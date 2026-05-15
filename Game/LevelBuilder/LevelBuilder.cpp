#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


bool inputtingLevelName = false;

/* --- LevelData --- */
const int levelSize = 10;	// The amount of tiles in the side length of the level (the level is always square)

std::vector<sf::Vector2i> squorbPath;
int tileIDs[levelSize][levelSize];	// The numerical IDs of the level tiles

// Generating the path the evil squorbs will take
void GeneratePath(sf::Vector2i startPos, sf::Vector2i endPos, int pathLength) {
	squorbPath.clear();
	sf::Vector2i currentPos = startPos;

	sf::Vector2i checkOffsets[4] = {	// List of up/down/left/right offsets so we can check the tiles around currentPos
		{0, -1},
		{1, 0},
		{0, 1},
		{-1, 0}
	};

	for (int i = 0; i < pathLength; i++) {
		for (sf::Vector2i checkOffset : checkOffsets) {
			sf::Vector2i checkingPos = currentPos + checkOffset;	// The position we're currently checking

			checkingPos.x = std::clamp(checkingPos.x, 0, levelSize);
			checkingPos.y = std::clamp(checkingPos.y, 0, levelSize);

			if (tileIDs[checkingPos.x][checkingPos.y] > 0 && tileIDs[checkingPos.x][checkingPos.y] < 4) {
				int previousInstructionIndex = std::clamp(i - 1, 0, pathLength);

				if (i > 0) {
					if (checkOffset * -1 != squorbPath.at(previousInstructionIndex)) {
						squorbPath.push_back(checkOffset);
						currentPos = checkingPos;
						break;
					}
				} else {
					squorbPath.push_back(checkOffset);
					currentPos = checkingPos;
					break;
				}
			}
		}
	}
}

/* --- Json --- */
using json = nlohmann::json;

// Credits to https://leapcell.io/blog/introduction-to-nlohmann-json#:~:text=4)%3B-,Custom%20Type%20Serialization,-To%20serialize%20and
// for showing how to convert a custom type to json
namespace sf {
	void to_json(nlohmann::json& j, const sf::Vector2i& v) {
		j = nlohmann::json{ v.x,v.y };
	}
}


void SaveLevelToJson(std::string levelName = "testLevel") {
	std::cout << "Saving " + levelName + " to json..." << std::endl;

	json j;

	j["levelSize"] = levelSize;
	j["squorbPath"] = squorbPath;
	j["tileIDs"] = tileIDs;

	std::ofstream output_file("../levels/" + levelName + ".json");
	if (output_file << j.dump()) {
		std::cout << "Saved" << std::endl;
	} else {
		std::cout << "Level save failed. Try renaming without special characters." << std::endl;
	}
	output_file.close();

}

int main() {
	sf::RenderWindow window(sf::VideoMode({ 1080, 1080 }), "Level Builder");
	window.setFramerateLimit(60);

	// Setup level variables
	const int tileSize = window.getSize().x / levelSize;

	int selectedTileID = 0;				// The currently selected tile ID for placing

	sf::RectangleShape tiles[levelSize][levelSize];		// The rectangleshapes to visualize the grid

	// Setup path generation variables
	int pathLength = 0;	// The length of the path the evil squorbs will take (used later for the basic flood fill)
	sf::Vector2i startPos;
	sf::Vector2i endPos;

	// Setup colors
	sf::Color backgroundColor = sf::Color(50, 50, 50);

	/* I know this might be sort of bad practice but this also doubles as the reference for which tile ID matches what tile type */
	std::unordered_map<int, sf::Color> colorIDs = {	// A dictionary to match a tile ID with a color, to make it slightly easier to visualize a level
		{0, backgroundColor},			// Empty space
		{1, sf::Color(0, 255, 0)},		// Start of the evil squorbs
		{2, sf::Color(255, 255, 255)},	// The path the evil squorbs take
		{3, sf::Color(255, 0, 0)},		// End of the evil squorbs
		//{4, sf::Color(128, 128, 200)},	// Playable area

	};

	// Fill the dictionaries with dummy values
	for (int y = 0; y < levelSize; y++) {
		for (int x = 0; x < levelSize; x++) {
			// Setup tileIDs
			tileIDs[x][y] = 0;
			
			// Setup tiles
			tiles[x][y].setSize(sf::Vector2f(tileSize, tileSize));
			tiles[x][y].setPosition(sf::Vector2f(x * tileSize, y * tileSize));
			tiles[x][y].setFillColor(backgroundColor);
			tiles[x][y].setOutlineThickness(1.0f);
			tiles[x][y].setOutlineColor(sf::Color::Black);
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

		// When any I/O or window event gets called
		while (const std::optional event = window.pollEvent()) {
			// Close the window if the X button (on the top right) is hit, or when Esc is pressed
			if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
				window.close();
			}

			// Handle text input
			if (inputtingLevelName) {
				if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
					if (textEvent->unicode == 8) {	// Backspace
						if (!userInput.empty()) userInput.pop_back();
					} else if (textEvent->unicode > 32 && textEvent->unicode < 128) {	// If the inputted character is within an acceptable range
						userInput += (char)(textEvent->unicode);
					}

					userTextInput.setString("Name: " + userInput);
				}
			}

			if (const auto* keyboardEvent = event->getIf<sf::Event::KeyPressed>()) {
				if (keyboardEvent->code >= sf::Keyboard::Key::Num0 && keyboardEvent->code <= sf::Keyboard::Key::Num9) {
					// Get the numerical position of the keycode enum, and subtract 26 since the position of num0 is 26 (which results in 0 equaling 0)
					int keyCode = static_cast<int>(keyboardEvent->code) - 26;

					// If "selectedTileID" is in the dictionary of colorIDs (a.k.a. if it's a valid tile)
					if (auto fillColor = colorIDs.find(keyCode); fillColor != colorIDs.end()) {
						selectedTileID = keyCode;
					}
				}
			}
		}

		int pathCount = 0;	// Variable that'll increment for
		// Loop over all the tiles on screen
		for (int y = 0; y < levelSize; y++) {
			for (int x = 0; x < levelSize; x++) {
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

				// Check if the mouse is in the bounds of the square it's currently checking
				if (tiles[x][y].getGlobalBounds().contains(mousePos)) {

					// Set the currently checking square's color to gr(a/e)y, a highlighting effect
					tiles[x][y].setFillColor(sf::Color(128, 128, 128));
					//std::cout << tileIDs[x][y] << std::endl;	// Check the tileID of the tile you're hovering over

					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) { // RMB is being held
						tileIDs[x][y] = 0;
					}
					// RMB check is placed before LMB check so if both are pressed, LMB (placing) takes priority

					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) { // LMB is being held
						tileIDs[x][y] = selectedTileID;
					}/* else if (pMousePressed) {	// LMB was released
						tileIDs[x][y] = selectedTileID;
					}*/

				} else if (auto fillColor = colorIDs.find(tileIDs[x][y]); fillColor != colorIDs.end()) {
					// If the mouse isn't on this square, try filling this square with the relevant color (based on the colorIDs map)

					tiles[x][y].setFillColor(fillColor->second);
				}

				// Run Logic depending on the tileID we're currently checking
				switch (tileIDs[x][y]) {
				case 1:		// Start tile
					pathCount++;
					startPos = { x, y };
					break;
				case 2:		// Path tile
					pathCount++;
					break;
				case 3:		// End tile
					pathCount++;
					endPos = { x, y };
					break;

				}
			}
		}
		pathLength = pathCount;

		pMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

		// Begin saving
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {	// While holding CTRL
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && !pSaveKeyPressed) {	// First frame of pressing S
				inputtingLevelName = true;
			}
			pSaveKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) && inputtingLevelName) {
			// Setup level data
			tileIDs;

			inputtingLevelName = false;
			std::string userTextInputString = userTextInput.getString();
			userTextInputString.erase(0, 6);
			GeneratePath(startPos, endPos, pathLength);
			SaveLevelToJson(userTextInputString);
		}

		window.clear();

		for (int x = 0; x < levelSize; x++) {
			for (int y = 0; y < levelSize; y++) {
				window.draw(tiles[x][y]); // Dereference the pointer to draw the object
			}
		}

		if (inputtingLevelName) {	// Draw text input only when inputting text
			window.draw(userTextInput);
		}

		window.display();
	}
}