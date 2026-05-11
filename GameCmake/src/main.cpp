#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
	auto desktop = sf::VideoMode::getDesktopMode();

	// Setup window
	sf::RenderWindow window(sf::VideoMode({ 1000, 1000 }), "SFML works!");
	sf::Vector2f windowSize = (sf::Vector2f)window.getSize();
	window.setFramerateLimit(60);

	auto bruh = 500.f;

	// Setup controllable circle
	sf::CircleShape circle(bruh);
	sf::CircleShape circle2(bruh);
	//circle.setFillColor(sf::Color::White);

	auto shader = sf::Shader{};
	if (!shader.loadFromFile("data/shader.frag", sf::Shader::Type::Fragment))
	{
		std::cerr << "Couldn't load fragment shader\n";
		return -1;
	}

	auto mouse_position = sf::Vector2f{};

	sf::Vector2f circlePos((desktop.size.x / 2.f) - circle.getRadius(), (desktop.size.y / 2.f) - circle.getRadius());
	float speed = 800.f;

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


	while (window.isOpen())
	{
		// Setup the delta seconds between frames and the fps value
		sf::Time dt = deltaClock.restart();
		float delta = dt.asSeconds();
		float fps = 1.f / delta;

		fpsText.setString("FPS: " + std::to_string((int)clock.getElapsedTime().asMilliseconds()));



		//circle.setRadius(bruh + 100 * sin(clock.getElapsedTime().asMilliseconds()/1000.f));




		mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition() - ((sf::Vector2i)window.getSize() / 2));

		window.setPosition({ (int)floor(mouse_position.x) ,(int)floor(mouse_position.y) });


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
			circlePos.y -= speed * delta;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
			circlePos.y += speed * delta;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			circlePos.x -= speed * delta;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			circlePos.x += speed * delta;

		sf::Vector2 globalCirclePos = circlePos - (sf::Vector2f)window.getPosition();

		circle.setPosition(globalCirclePos);
		circle2.setPosition({ globalCirclePos.x + 500, globalCirclePos.y });

		circle.setScale({0.5,0.5});


		while (const std::optional event = window.pollEvent())
		{
			if (event->is < sf::Event::Resized>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
				window.close();
			}
		}

		shader.setUniform("circle_pos", sf::Vector2(globalCirclePos.x + ((window.getSize().x / 2.f)), window.getSize().y - (globalCirclePos.y + (window.getSize().y / 2.f))));
		shader.setUniform("circle_radius", circle.getRadius());

		//shader.setUniform("u_resolution", sf::Glsl::Vec2{ window.getSize() });
		//shader.setUniform("u_mouse", sf::Glsl::Vec2{ mouse_position });
		//shader.setUniform("u_time", clock.getElapsedTime().asSeconds());

		window.clear();
		window.draw(circle, &shader);
		window.draw(circle2);
		window.draw(fpsText);
		window.display();
	}
}