#pragma once
#include "Zoom.h"
#include "Menu.h"
#include "FileModule.h"

#include <SFML/Graphics.hpp>
#include <vector>

class GameOfLife
{
public:
	GameOfLife();
	void run();

	static const std::string sTitle;
	static const size_t sGridSize;
	static const sf::Color sAliveColor;
	static const sf::Color sDeadColor;
	static const sf::Color sBackgroundColor;	
	static const sf::Time sTimePerFrame;
	

private:
	void handleFileEvent();
	void handleEvent();
	void update(sf::Time dt);
	void render();

	void init();
	void initZoomArea();
	void handleKeyPressed(const sf::Keyboard::Key &key);
	void resetInfos();
	void updateGrid();
	void updateVertices();
	void populate(int ratio);
	void reverse_grid();
	void speedUpDt(bool accelerate);

	sf::RenderWindow mWindow;
	sf::RectangleShape mSeparator;
	sf::Font mFont;
	std::vector<bool> mCellsMatrix;
	std::vector<bool> mTempCellsMatrix;
	sf::VertexArray mGrid;
	sf::Vector2i mZoomPosition;
	FileModule mFileModule;
	std::string mBuffer;

	int mGeneration;
	bool mIsPaused;
	Zoom mZoom;
	sf::RectangleShape mZoomArea;
	Menu mMenu;

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



