#pragma once
#include <SFML/Graphics.hpp>

class Zoom
{
public:
	Zoom(std::vector<bool> &cellsMatrix);
	void in(int x, int y);
	void out();
	bool isActive() const;
	void draw(sf::RenderWindow &window);
	sf::Vector2i updateGrid(sf::Vector2i mousePosition);

private:
	bool mIsActive;
	sf::Vector2i mCenter;
	std::vector<bool> &mCellsMatrix;
	int mOffsetX;
	int mOffsetY;
};
