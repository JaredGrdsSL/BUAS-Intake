#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
	using json = nlohmann::json;

	auto desktop = sf::VideoMode::getDesktopMode();

	// Setup the window
	sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Squorb");

	sf::Vector2f windowSize = (sf::Vector2f)window.getSize();

	window.setFramerateLimit(60);

	auto bruh = 500.f;

	// Setup controllable circle
	sf::CircleShape circle(bruh);
	circle.setFillColor(sf::Color::White);

	// Setup Shader
	//auto shader = sf::Shader{};
	//if (!shader.loadFromFile("shaders/metaballs.frag", sf::Shader::Type::Fragment))
	//{
	//	std::cerr << "Couldn't load fragment shader\n";
	//	return -1;
	//}

	//auto screenSquare

	auto mouse_position = sf::Vector2f{};

	sf::Vector2f circlePos((desktop.size.x / 2.f) - circle.getRadius(), (desktop.size.y / 2.f) - circle.getRadius());
	float speed = 500.f;

	// Create a font
	sf::Font defaultFont;
	if (!defaultFont.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
		std::cout << "Error Loading Font.";
	}

	// Create text object(s)
	sf::Text fpsText(defaultFont, "FPS: 0", 24);
	fpsText.setFillColor(sf::Color::White);
	fpsText.setPosition({ 10, 10 });

	// Variables for timing
	sf::Clock clock;
	sf::Clock deltaClock;
	double lastTime = 0.f;

	while (window.isOpen()) {
		// Setup the delta seconds between frames and the fps value
		sf::Time dt = deltaClock.restart();
		float delta = dt.asSeconds();
		float fps = 1.f / delta;

		fpsText.setString("FPS: " + std::to_string((int)fps));



		//circle.setRadius(bruh + 100 * sin(clock.getElapsedTime().asMilliseconds()/1000.f));




		//mouse_position = window1.mapPixelToCoords(sf::Mouse::getPosition() - ((sf::Vector2i)window1.getSize() / 2));
		mouse_position = (sf::Vector2f)(sf::Mouse::getPosition() - ((sf::Vector2i)window.getSize() / 2));

		window.setPosition({ (int)floor(mouse_position.x) ,(int)floor(mouse_position.y) });

		window.setSize({
			(unsigned int)std::clamp(windowSize.x, 384.f, 1920.f),
			(unsigned int)std::clamp(windowSize.y, 216.f, 1080.f)
			});

		sf::FloatRect visibleArea({ 0, 0 }, (sf::Vector2f)window.getSize());
		window.setView(sf::View(visibleArea));


		//std::cout << window1Size.x << ", " << window1Size.y << std::endl;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
			windowSize.y += speed * delta;
		//circlePos.y -= speed * delta;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
			windowSize.y -= speed * delta;
		//circlePos.y += speed * delta;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			windowSize.x -= speed * delta;
		//circlePos.x -= speed * delta;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			windowSize.x += speed * delta;
		//circlePos.x += speed * delta;

		sf::Vector2 globalCirclePos = circlePos - (sf::Vector2f)window.getPosition();

		circle.setPosition(globalCirclePos);
		//circle2.setPosition({ globalCirclePos.x + 500, globalCirclePos.y });

		circle.setScale({ 0.5,0.5 });


		while (const std::optional event = window.pollEvent()) {
			if (event->is < sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
				window.close();
			}
		}

		//shader.setUniform("circle_pos", sf::Vector2(globalCirclePos.x + ((window1.getSize().x / 2.f)), window1.getSize().y - (globalCirclePos.y + (window1.getSize().y / 2.f))));
		//shader.setUniform("circle_radius", circle.getRadius());

		//shader.setUniform("u_resolution", sf::Glsl::Vec2{ window.getSize() });
		//shader.setUniform("u_mouse", sf::Glsl::Vec2{ mouse_position });
		//shader.setUniform("u_time", clock.getElapsedTime().asSeconds());

		window.clear();
		window.draw(circle);
		//window.draw(circle2);
		window.draw(fpsText);
		window.display();

		//window2.clear();
		//window2.draw(circle);
		////window.draw(circle2);
		//window2.draw(fpsText);
		//window2.display();
	}
}