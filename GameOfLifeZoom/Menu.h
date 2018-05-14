#pragma once
#include <SFML/Graphics.hpp>
class Menu
{
public:
	Menu(sf::Font &font);
	void draw(sf::RenderWindow &window, int generation, int speed, int zoomLevel, bool isZoom, bool isPaused);

private:
	sf::Font &mFont;
};

