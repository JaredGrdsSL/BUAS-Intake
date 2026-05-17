#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>

#include <headers/Squorb.h>
#include <headers/Tower.h>

// Credits to https://leapcell.io/blog/introduction-to-nlohmann-json#:~:text=4)%3B-,Custom%20Type%20Serialization,-To%20serialize%20and
// for showing how to convert a custom type to json
namespace sf {
	void from_json(const nlohmann::json& j, sf::Vector2i& v) {
		if (!j.is_array() || j.size() != 2) {
			throw nlohmann::json::type_error::create(302, "Validation failed: Expected array of size 2", &j);
		}
		v.x = j.at(0).get<int>();
		v.y = j.at(1).get<int>();
	}

	sf::Vector2f lerp(const sf::Vector2f& start, const sf::Vector2f& end, float t) {
		return { std::lerp(start.x, end.x, t), std::lerp(start.y, end.y, t) };
	}

	sf::Vector2f floor(const sf::Vector2f& v) {
		return { std::floorf(v.x), std::floorf(v.y) };
	}
}

// Level loading data
std::vector<sf::Vector2i> squorbPath;

const int levelSize = 10;
int tileIDs[levelSize][levelSize];

// JSON
using json = nlohmann::json;

void LoadLevelFromJson(std::string filePath) {
	std::ifstream file(filePath);

	json j = json::parse(file);

	auto& tileData = j["tileIDs"];

	for (int i = 0; i < levelSize; i++) {
		for (int j_idx = 0; j_idx < levelSize; j_idx++) {
			// Access by index and assign to your raw array
			tileIDs[i][j_idx] = tileData[i][j_idx].get<int>();
		}
	}

	squorbPath = j.at("squorbPath");
}

int main() {
	LoadLevelFromJson("../levels/TestLevel.json");

	auto desktop = sf::VideoMode::getDesktopMode();

	int tileSize = 108;

	// Setup the window
	sf::RenderWindow window(sf::VideoMode({ 400, 400 }), "Squorb");
	sf::Vector2f windowSize = (sf::Vector2f)window.getSize();
	window.setFramerateLimit(60);

	sf::Vector2f globalMousePos;

	// Setup the level grid
	std::vector<sf::RectangleShape> levelTiles;
	sf::Vector2f levelGridPos = {
		(static_cast<float>(desktop.size.x) - levelSize * tileSize) / 2.f,
		(static_cast<float>(desktop.size.y) - levelSize * tileSize) / 2.f
	};

	sf::Vector2f startingTilePos;	// The position of the starting tile, helpful for spawning the squorbs
	int endingTileIndex;			// The index of the ending tile, index instead of vector because it needs to reference levelTiles

	for (int i = 0; i < levelSize; i++) {
		for (int j = 0; j < levelSize; j++) {
			sf::RectangleShape tempRect;

			tempRect.setSize({ (float)tileSize,(float)tileSize });
			// No need to set position since that's done in update
			tempRect.setOutlineColor(sf::Color(30, 30, 20));
			tempRect.setOutlineThickness(1.0f);

			switch (tileIDs[j][i]) {
			case 0: default:
				tempRect.setFillColor(sf::Color(40, 60, 50));
				tempRect.setOutlineColor(sf::Color(128, 128, 128));
				break;
			case 1:
				tempRect.setFillColor(sf::Color(128, 255, 128));

				startingTilePos = levelGridPos + sf::Vector2f((float)j * tileSize, (float)i * tileSize);
				break;
			case 2:
				tempRect.setFillColor(sf::Color(60, 60, 40));
				break;
			case 3:
				tempRect.setFillColor(sf::Color(128, 50, 50));

				endingTileIndex = j + (i * levelSize);
				break;
			}

			levelTiles.push_back(tempRect);
		}
	}

	// Setup Shader
	//auto shader = sf::Shader{};
	//if (!shader.loadFromFile("shaders/metaballs.frag", sf::Shader::Type::Fragment))
	//{
	//	std::cerr << "Couldn't load fragment shader\n";
	//	return -1;
	//}
	//auto screenSquare


	// Squorb variables
	std::vector<Squorb> squorbs;
	auto squorbSize = 25.f;
	sf::Vector2f squorbStartPos(startingTilePos + sf::Vector2f{ (float)tileSize / 2.f, (float)tileSize / 2.f });

	// Tower variables
	std::vector<Tower> towers;
	auto towerSize = 30.f;

	// Create a font
	sf::Font defaultFont;
	if (!defaultFont.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
		std::cout << "Error Loading Font.";
	}

	// Create text object(s)
	sf::Text fpsText(defaultFont, "FPS: ", 24);
	fpsText.setFillColor(sf::Color::White);
	fpsText.setPosition({ 10, 10 });

	// Variables for timing
	sf::Clock clock;
	sf::Clock deltaClock;
	double lastTime = 0.f;

	float t = 0;
	int pT = 0;			// Value of t last frame


	while (window.isOpen()) {
		// Window refresh
		window.clear();

		// Draw this first so it's on the "background"
		for (int i = 0; i < levelSize * levelSize; i++) {
			window.draw(levelTiles[i]);
		}

		// Setup the delta seconds between frames and the fps value
		sf::Time dt = deltaClock.restart();
		float delta = dt.asSeconds();
		float fps = 1.f / delta;
		t += delta;

		fpsText.setString("FPS: " + std::to_string((int)fps));

		globalMousePos = (sf::Vector2f)(sf::Mouse::getPosition() - ((sf::Vector2i)window.getSize() / 2));

		window.setPosition({ (int)floor(globalMousePos.x) ,(int)floor(globalMousePos.y) });

		window.setSize({
			(unsigned int)std::clamp(windowSize.x, 384.f, 1920.f),
			(unsigned int)std::clamp(windowSize.y, 216.f, 1080.f)
			});

		sf::FloatRect visibleArea({ 0, 0 }, (sf::Vector2f)window.getSize());
		window.setView(sf::View(visibleArea));

		// Poll events
		while (const std::optional event = window.pollEvent()) {
			if (event->is < sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
				window.close();
			}

			if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (mousePressed->button == sf::Mouse::Button::Left) {
					// Tower placement logic
					for (int i = 0; i < levelTiles.size(); i++) {
						if (levelTiles[i].getGlobalBounds().contains((sf::Vector2f)sf::Mouse::getPosition(window))) {
							if (tileIDs[i % levelSize][i / levelSize] == 0) {	// Allow tower placement only if the tileID is 0 (default)
								Tower tower(towerSize, (sf::Vector2f)sf::Mouse::getPosition(), sf::Color::Yellow, squorbs);
								towers.push_back(tower);
							}
						}
					}
				}
			}
		}

		// Set global positions
		for (int i = 0; i < levelSize * levelSize; i++) {
			sf::Vector2f tilePos = {
				std::fmodf(static_cast<float>(i), (float)levelSize) * tileSize,
				std::floorf(static_cast<float>(i) / (float)levelSize) * tileSize
			};

			levelTiles[i].setPosition(levelGridPos + tilePos - (sf::Vector2f)window.getPosition());
		}

		// Handle Squorbs
		if (squorbs.size() > 0) {
			for (Squorb& squorb : squorbs) {
				squorb.walkAlongPath(delta, (sf::Vector2f)window.getPosition(), (float)tileSize);	// A sort of "update" function for the squorb

				if (levelTiles[endingTileIndex].getGlobalBounds().contains(squorb.getPosition())) {	// If on the final tile
					squorb.kill();	// Simply marks it for death, killing is done at the end of main
				}
			}
		}

		// Handle Towers
		if (towers.size() > 0) {
			for (Tower& tower : towers) {
				tower.update(delta, window);
			}
		}

		// Spawn a squorb every few seconds
		if ((int)std::floorf(t) % 5 == 1 && (int)std::floorf(t) != pT) {
			Squorb squorb(squorbSize, squorbStartPos, sf::Color::Black, squorbPath);
			squorbs.push_back(squorb);
		}
		pT = t;

		// Draw squorbs
		if (squorbs.size() > 0) {
			for (Squorb squorb : squorbs) {
				squorb.draw(window);
			}
		}

		// Draw towers
		if (towers.size() > 0) {
			for (Tower tower : towers) {
				tower.draw(window);
			}
		}

		// Kill any squorbs marked for death
		std::erase_if(squorbs, [](const Squorb& s) {
			return !s.isAlive();
			});

		// Draw this last so it's on the "foreground"
		window.draw(fpsText);
		window.display();
	}
}