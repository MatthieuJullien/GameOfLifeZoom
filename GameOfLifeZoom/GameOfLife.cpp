#include "GameOfLife.h"
#include <random>
#include <cassert>

const std::string GameOfLife::mTitle = "Game of Life";
const size_t GameOfLife::mGridSize = 600;//600
const sf::Color GameOfLife::mAliveColor = sf::Color::Green;
const sf::Color GameOfLife::mDeadColor = sf::Color::Black;
const sf::Color GameOfLife::mBackgroundColor = sf::Color(128, 128, 128, 255);
const int GameOfLife::fractionOfTheGrid = 5;
const sf::Time GameOfLife::mTimePerFrame = sf::seconds(1.f / 60.f);

GameOfLife::GameOfLife()
	: mWindow(sf::VideoMode(800, 600), mTitle, sf::Style::Titlebar | sf::Style::Close)
	, mSeparator()
	, mCellsMatrix(mGridSize * mGridSize, false)
	, mTempCellsMatrix(mGridSize * mGridSize, false)
	, mGrid(sf::Points, mGridSize * mGridSize)
	, mIsPaused(true)
	, mZoom(mCellsMatrix)
	, mZoomArea()
	, mGeneration(0)
	, mSpeed(fast)
	, mAccu(sf::Time::Zero)
{
	init();
	resetInfos();
}

void GameOfLife::init()
{
	mSeparator = sf::RectangleShape(sf::Vector2f(2, mGridSize));
	mSeparator.setPosition(mGridSize - 1, 0);
	mSeparator.setFillColor(sf::Color::Red);

	float zoomAreaSize = mGridSize / fractionOfTheGrid;
	mZoomArea = sf::RectangleShape(sf::Vector2f(zoomAreaSize, zoomAreaSize));
	mZoomArea.setOrigin(sf::Vector2f(zoomAreaSize / 2, zoomAreaSize / 2));
	mZoomArea.setOutlineThickness(2);
	mZoomArea.setOutlineColor(sf::Color::Red);
	mZoomArea.setFillColor(sf::Color::Transparent);

	for (int h = 0; h < mGridSize; ++h) {
		for (int w = 0; w < mGridSize; ++w) {
			mGrid[w + h * mGridSize].position = sf::Vector2f(w, h);
			mGrid[w + h * mGridSize].color = mDeadColor;
		}
	}
}

void GameOfLife::resetInfos()
{
	mGeneration = 0;
	std::string title = mTitle + " : 0";
	mWindow.setTitle(title);
	mSpeed = fast;
	mDelay = sf::seconds(1.f / 60.f);
	mAccu = sf::Time::Zero;
}

void GameOfLife::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > mTimePerFrame)
		{
			timeSinceLastUpdate -= mTimePerFrame;
			handleEvent();
			update(mTimePerFrame);
		}
		render();
	}
}

void GameOfLife::handleEvent()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				mWindow.close();
				break;
			case sf::Keyboard::Return:
			case sf::Keyboard::Space:
				mIsPaused = !mIsPaused;
				break;
			case sf::Keyboard::Subtract:
				speedUpDt(false);
				break;
			case sf::Keyboard::Add:
				speedUpDt(true);
				break;
			case sf::Keyboard::Num0:
			case sf::Keyboard::Numpad0:
				populate(0);
				break;
			case sf::Keyboard::Num1:
			case sf::Keyboard::Numpad1:
				populate(1);
				break;
			case sf::Keyboard::Num2:
			case sf::Keyboard::Numpad2:
				populate(2);
				break;
			case sf::Keyboard::Num3:
			case sf::Keyboard::Numpad3:
				populate(3);
				break;
			case sf::Keyboard::Num4:
			case sf::Keyboard::Numpad4:
				populate(4);
				break;
			case sf::Keyboard::Num5:
			case sf::Keyboard::Numpad5:
				populate(5);
				break;
			case sf::Keyboard::Num6:
			case sf::Keyboard::Numpad6:
				populate(6);
				break;
			case sf::Keyboard::Num7:
			case sf::Keyboard::Numpad7:
				populate(7);
				break;
			case sf::Keyboard::Num8:
			case sf::Keyboard::Numpad8:
				populate(8);
				break;
			case sf::Keyboard::Num9:
			case sf::Keyboard::Numpad9:
				populate(9);
				break;
			case sf::Keyboard::S:
				if (mIsPaused)
				{
					updateGrid();
					++mGeneration;
					std::string title = mTitle + " : " + std::to_string(mGeneration);
					mWindow.setTitle(title);
					updateVertices();
				}
				break;
			case sf::Keyboard::I:
				resetInfos();
				mIsPaused = true;
				mZoom.out();
				reverse_grid();
				break;
			default:
				break;
			}
		}
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Right)
			{
				if (!mZoom.isActive())
				{
					mZoomPosition = sf::Mouse::getPosition(mWindow);
					mZoom.in(mZoomPosition.x, mZoomPosition.y);
				}
				else
				{
					mZoom.out();
					sf::Mouse::setPosition(mZoomPosition, mWindow);
				}
			}
			else if (event.mouseButton.button == sf::Mouse::Left)
			{
				sf::Vector2i pos = sf::Mouse::getPosition(mWindow);
				if (!mZoom.isActive())
				{
					if (pos.x < mGridSize && pos.y < mGridSize)
					{
						mCellsMatrix[pos.x + pos.y * mGridSize] = !mCellsMatrix[pos.x + pos.y * mGridSize];
						mGrid[pos.x + pos.y * mGridSize].color = mCellsMatrix[pos.x + pos.y * mGridSize] ? mAliveColor : mDeadColor;
					}
				}
				else
				{
					if (pos.x < mGridSize && pos.y < mGridSize)
					{
						sf::Vector2i coord = mZoom.updateGrid(pos);
						if (coord != sf::Vector2i(-1, -1))
						{
							mGrid[coord.x + coord.y * mGridSize].color = mCellsMatrix[coord.x + coord.y * mGridSize] ? mAliveColor : mDeadColor;
						}
					}
				}

			}
		}
		else if (event.type == sf::Event::Closed)
		{
			mWindow.close();
		}
	}
}

void GameOfLife::update(sf::Time dt)
{
	if (!mIsPaused)
	{
		mAccu += dt;
		if (mAccu > mDelay)
		{
			mAccu = sf::Time::Zero;
			updateGrid();
			++mGeneration;
			std::string title = mTitle + " : " + std::to_string(mGeneration);
			mWindow.setTitle(title);
			updateVertices();
		}
	}
	sf::Vector2i mousePosition = sf::Mouse::getPosition(mWindow);
	mZoomArea.setPosition(static_cast<sf::Vector2f>(mousePosition));
}

void GameOfLife::render()
{
	mWindow.clear(mBackgroundColor);
	if (!mZoom.isActive())
	{
		mWindow.draw(mGrid);
		mWindow.draw(mZoomArea);
	}
	else
	{
		mZoom.draw(mWindow);
	}
	mWindow.draw(mSeparator);
	mWindow.display();
}

void GameOfLife::updateGrid()
{
	int nb_neighbor;
	int indexWidthMax = mGridSize - 1;
	int indexHeightMax = mGridSize - 1;
	for (int h = 0; h < mGridSize; ++h)
	{
		for (int w = 0; w < mGridSize; ++w)
		{
			nb_neighbor = 0;
			//line above
			if (w > 0 && h > 0 && mCellsMatrix[w - 1 + mGridSize * (h - 1)])
				++nb_neighbor;
			if (h > 0 && mCellsMatrix[w + mGridSize * (h - 1)])
				++nb_neighbor;
			if (w < indexWidthMax && h > 0 && mCellsMatrix[w + 1 + mGridSize * (h - 1)])
				++nb_neighbor;
			//same line
			if (w > 0 && mCellsMatrix[w - 1 + mGridSize * h])
				++nb_neighbor;
			if (w < indexWidthMax && mCellsMatrix[w + 1 + mGridSize * h])
				++nb_neighbor;
			//line below
			if (w > 0 && h < indexHeightMax && mCellsMatrix[w - 1 + mGridSize * (h + 1)])
				++nb_neighbor;
			if (h < indexHeightMax && mCellsMatrix[w + mGridSize * (h + 1)])
				++nb_neighbor;
			if (w < indexWidthMax && h < indexHeightMax && mCellsMatrix[w + 1 + mGridSize * (h + 1)])
				++nb_neighbor;
			//cell update
			if (!mCellsMatrix[w + mGridSize * h] && nb_neighbor == 3)
				mTempCellsMatrix[w + mGridSize * h] = true;
			else if (mCellsMatrix[w + mGridSize * h] && (nb_neighbor < 2 || nb_neighbor > 3))
				mTempCellsMatrix[w + mGridSize * h] = false;
			else
				mTempCellsMatrix[w + mGridSize * h] = mCellsMatrix[w + mGridSize * h];
		}
	}
	mCellsMatrix.swap(mTempCellsMatrix);
}

void GameOfLife::updateVertices()
{
	int index;
	for (int h = 0; h < mGridSize; ++h)
	{
		for (int w = 0; w < mGridSize; ++w)
		{
			index = w + h * mGridSize;
			mGrid[index].color = mCellsMatrix[index] ? mAliveColor : mDeadColor;
		}
	}
}

void GameOfLife::populate(int ratio)
{
	if (mIsPaused)
	{
		resetInfos();
		std::random_device r;
		std::default_random_engine e1(r());
		std::uniform_int_distribution<int> uniform_dist(0, 9);
		for (int h = 0; h < mGridSize; ++h)
		{
			for (int w = 0; w < mGridSize; ++w)
			{
				int alea = uniform_dist(e1);
				mCellsMatrix[w + h * mGridSize] = alea < ratio ? true : false;
				//mGrid[w + h * mGridSize].position = sf::Vector2f(w, h);
			}
		}
		updateVertices();
	}
}

void GameOfLife::reverse_grid()
{
	assert(mIsPaused);
	for (int h = 0; h < mGridSize; ++h)
	{
		for (int w = 0; w < mGridSize; ++w)
		{
			mCellsMatrix[w + h * mGridSize] = !mCellsMatrix[w + h * mGridSize];
		}
	}
	updateVertices();
}

void GameOfLife::speedUpDt(bool accelerate)
{
	if (accelerate)
	{
		mSpeed = (mSpeed + 1) > fastest ? fastest : mSpeed + 1;
	}
	else
	{
		mSpeed = (mSpeed - 1) < slowest ? slowest : mSpeed - 1;
	}
	switch (mSpeed)
	{
	case slowest:
		mDelay = sf::seconds(1.f / 10.f);
		break;
	case slower:
		mDelay = sf::seconds(1.f / 15.f);
		break;
	case slow:
		mDelay = sf::seconds(1.f / 30.f);
		break;
	case medium:
		mDelay = sf::seconds(1.f / 45.f);
		break;
	case fast:
		mDelay = sf::seconds(1.f / 60.f);
		break;
	case faster:
		mDelay = sf::seconds(1.f / 90.f);
		break;
	case fastest:
		mDelay = sf::seconds(1.f / 120.f);
		break;
	default:
		break;
	}
}