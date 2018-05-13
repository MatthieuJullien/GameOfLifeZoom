#pragma once
#include <SFML/Graphics.hpp>
class Menu
{
public:
	void draw(sf::RenderWindow &window, int generation, int speed, int zoomLevel, bool isZoom, bool isPaused);
};

