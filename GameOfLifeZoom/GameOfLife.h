#pragma once
#include "Zoom.h"

#include <SFML/Graphics.hpp>

class GameOfLife
{
public:
	GameOfLife();
	void run();

	static const std::string mTitle;
	static const size_t mGridSize;
	static const sf::Color mAliveColor;
	static const sf::Color mDeadColor;
	static const sf::Color mBackgroundColor;
	static const int fractionOfTheGrid;
	static const sf::Time mTimePerFrame;

private:
	void handleEvent();
	void update(sf::Time dt);
	void render();

	void init();
	void resetInfos();
	void updateGrid();
	void updateVertices();
	void populate(int ratio);
	void reverse_grid();
	void speedUpDt(bool accelerate);

	sf::RenderWindow mWindow;
	sf::RectangleShape mSeparator;
	std::vector<bool> mCellsMatrix;
	std::vector<bool> mTempCellsMatrix;
	sf::VertexArray mGrid;
	sf::Vector2i mZoomPosition;

	int mGeneration;
	bool mIsPaused;
	Zoom mZoom;
	sf::RectangleShape mZoomArea;

	enum Speed
	{
		slowest,
		slower,
		slow,
		medium,
		fast,
		faster,
		fastest
	};
	sf::Time mDelay;
	sf::Time mAccu;
	
	int mSpeed;
};



