#include "game.h"
#include "surface.h"
#include "windows.h"
#include <string>
#include <iostream>
#include <cstdio> //printf

namespace Tmpl8
{
	Surface* screen;

	// Physics
	float gravity = 1.6f;

	bool grounded = false;
	bool hitWallLeft = false;
	bool hitWallRight = false;

	// Sprite rendering
	Sprite sprite(new Surface("assets/ball.png"), 1);
	int frame = 0;

	int spriteWidth;
	int spriteHeight;

	// Sprite movement
	float spriteSpeed = 1;

	float spriteXVel;
	float spriteYVel;

	int spriteX;
	int spriteY;

	// Setup the direction keys
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	// -----------------------------------------------------------
	// Initialize the application (Runs on startup)
	// -----------------------------------------------------------
	void Game::Init()
	{
		spriteWidth = sprite.GetWidth();
		spriteHeight = sprite.GetHeight();
	}

	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{
	}

	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick(float deltaTime)
	{
		// clear the graphics window
		screen->Clear(0);

		Movement();

		// draw a sprite
		//sprite.SetFrame(frame);
		sprite.Draw(screen, spriteX, spriteY);
		//if (++frame == 36) frame = 0;
	}

	void Game::Movement() {
		// Map the WASD keys to their respective boolean
		up = GetAsyncKeyState('W');
		down = GetAsyncKeyState('S');
		left = GetAsyncKeyState('A');
		right = GetAsyncKeyState('D');

		// Add or subtract from the velocity base on the movement direction
		//if (up) { spriteYVel -= spriteSpeed; }
		if (down) { spriteYVel += spriteSpeed; }
		if (left) { spriteXVel -= spriteSpeed; }
		if (right) { spriteXVel += spriteSpeed; }

		// Do grounded check before applying gravity
		grounded = spriteY + spriteHeight >= screen->GetHeight();
		char coordinatesToString[64];
		std::sprintf(coordinatesToString, "%d, %d", spriteX, spriteY);
		screen->Print(coordinatesToString, 2, 2, 0xffffff);

		// Apply gravity
		if (!grounded) {
			spriteYVel += gravity;
		}
		else {
			spriteYVel *= -spriteYVel/gravity;
			spriteY = screen->GetHeight() - spriteHeight; // clamp to ground
		}

		hitWallLeft = spriteX < 0;
		hitWallRight = spriteX + spriteWidth > screen->GetWidth();

		// Wall collision
		if (hitWallLeft) {
			spriteXVel = 0;
			spriteX = 0;
		}

		if (hitWallRight) {
			spriteXVel = 0;
			spriteX = screen->GetWidth() - spriteWidth;
		}

		// Multiply speed by a number < 1 as a sort of air resistance/friction
		spriteXVel *= 0.93f + (grounded * -0.05f);
		spriteYVel *= 0.93f + (grounded * -0.05f);

		// Add the velocity to the position
		spriteX += (int)spriteXVel;
		spriteY += (int)spriteYVel;
	}
};