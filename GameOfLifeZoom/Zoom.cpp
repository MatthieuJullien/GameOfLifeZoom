#include "Zoom.h"
#include "GameOfLife.h"

Zoom::Zoom(std::vector<bool> &cellsMatrix)
	: mCellsMatrix(cellsMatrix)
	, mIsActive(false)
	, mOffsetX(0)
	, mOffsetY(0)
{}

void Zoom::in(int x, int y)
{
	mIsActive = true;
	mCenter = sf::Vector2i(x, y);
}

void Zoom::out()
{
	mIsActive = false;
}

bool Zoom::isActive() const
{
	return mIsActive;
}

void Zoom::draw(sf::RenderWindow &window)
{
	int gridSize = GameOfLife::mGridSize;
	int zoomAreaSize = gridSize / GameOfLife::fractionOfTheGrid;
	int secondZoomAreaHalf = zoomAreaSize / 2;
	int firstZoomAreaHalf = secondZoomAreaHalf - 1;

	int zoomAreaWidth = mCenter.x;
	if (zoomAreaWidth - firstZoomAreaHalf < 0)
	{
		zoomAreaWidth = zoomAreaWidth + secondZoomAreaHalf;
	}
	else if (zoomAreaWidth + secondZoomAreaHalf > gridSize)
	{
		zoomAreaWidth = gridSize - zoomAreaWidth + firstZoomAreaHalf;
	}
	else
	{
		zoomAreaWidth = zoomAreaSize;
	}

	int zoomAreaHeight = mCenter.y;
	if (zoomAreaHeight - firstZoomAreaHalf < 0)
	{
		zoomAreaHeight = zoomAreaHeight + secondZoomAreaHalf;
	}
	else if (zoomAreaHeight + secondZoomAreaHalf > gridSize)
	{
		zoomAreaHeight = gridSize - zoomAreaHeight + firstZoomAreaHalf;
	}
	else
	{
		zoomAreaHeight = zoomAreaSize;
	}

	int cellsMatrixSize = GameOfLife::mGridSize;

	int cellSize = GameOfLife::fractionOfTheGrid;

	int hStart = (mCenter.y - firstZoomAreaHalf) > 0 ? (mCenter.y - firstZoomAreaHalf) : 0;
	int wStart = (mCenter.x - firstZoomAreaHalf) > 0 ? (mCenter.x - firstZoomAreaHalf) : 0;
	mOffsetX = mCenter.x < firstZoomAreaHalf ? std::abs(mCenter.x - firstZoomAreaHalf) * cellSize : 0;
	mOffsetY = mCenter.y < firstZoomAreaHalf ? std::abs(mCenter.y - firstZoomAreaHalf) * cellSize : 0;

	for (int h = 0; h < zoomAreaHeight; ++h)
	{
		for (int w = 0; w < zoomAreaWidth; ++w) {
			sf::Vector2f cellPosition(w * cellSize + mOffsetX, h * cellSize + mOffsetY);
			int index = w + wStart + (h + hStart) * gridSize;
			sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
			sf::Color cellColor = mCellsMatrix[index] ? GameOfLife::mAliveColor : GameOfLife::mDeadColor;
			cell.setFillColor(cellColor);
			cell.setPosition(cellPosition);
			window.draw(cell);
		}
	}
}

sf::Vector2i Zoom::updateGrid(sf::Vector2i mousePosition)
{
	if (mousePosition.x < mOffsetX || mousePosition.y < mOffsetY)
	{
		return sf::Vector2i(-1, -1);
	}
	int gridSize = GameOfLife::mGridSize;
	int firstZoomAreaHalf = gridSize / GameOfLife::fractionOfTheGrid / 2 - 1;

	int cellSize = GameOfLife::fractionOfTheGrid;
	int x = mousePosition.x / cellSize;
	int y = mousePosition.y / cellSize;

	x = x - firstZoomAreaHalf + mCenter.x;
	y = y - firstZoomAreaHalf + mCenter.y;

	if (x >= gridSize || y >= gridSize)
	{
		return sf::Vector2i(-1, -1);
	}

	int index = x + y * gridSize;
	mCellsMatrix[index] = !mCellsMatrix[index];
	return sf::Vector2i(x, y);
}