#pragma once
#include "GameOfLife.h"

#include <SFML/Graphics.hpp>

class Zoom
{
public:
	Zoom(const GameOfLife &game, std::vector<bool> &cellsMatrix);
	void in(int x, int y);
	void out();
	bool isActive() const;
	void draw(sf::RenderWindow &window);
	sf::Vector2i updateGrid(sf::Vector2i mousePosition);

private:
	bool mIsActive;
	sf::Vector2i mCenter;
	const GameOfLife &mGame;
	std::vector<bool> &mCellsMatrix;
	int mOffsetX;
	int mOffsetY;
};
