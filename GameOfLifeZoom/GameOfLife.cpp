#include "GameOfLife.h"
#include <random>
#include <cassert>

const std::string GameOfLife::sTitle = "Game of Life";
const size_t GameOfLife::sGridSize = 600;//600
const sf::Color GameOfLife::sAliveColor = sf::Color::Green;
const sf::Color GameOfLife::sDeadColor = sf::Color::Black;
const sf::Color GameOfLife::sBackgroundColor = sf::Color(128, 128, 128, 255);
const sf::Time GameOfLife::sTimePerFrame = sf::seconds(1.f / 240.f);

GameOfLife::GameOfLife()
	: mWindow(sf::VideoMode(1000, 600), sTitle, sf::Style::Titlebar | sf::Style::Close)
	, mSeparator()
	, mFont()
	, mCellsMatrix(sGridSize * sGridSize, false)
	, mTempCellsMatrix(sGridSize * sGridSize, false)
	, mGrid(sf::Points, sGridSize * sGridSize)
	, mIsPaused(true)
	, mZoom(mCellsMatrix)
	, mZoomArea()
	, mFileModule(mWindow, mFont)
	, mMenu(mFont)
	, mGeneration(0)
	, mSpeed(fast)
	, mAccu(sf::Time::Zero)
	, mBuffer()
{
	init();
	resetInfos();
}

void GameOfLife::init()
{
	mSeparator = sf::RectangleShape(sf::Vector2f(2, sGridSize));
	mSeparator.setPosition(sGridSize - 1, 0);
	mSeparator.setFillColor(sf::Color::Red);

	initZoomArea();

	for (int h = 0; h < sGridSize; ++h) {
		for (int w = 0; w < sGridSize; ++w) {
			mGrid[w + h * sGridSize].position = sf::Vector2f(w, h);
			mGrid[w + h * sGridSize].color = sDeadColor;
		}
	}

	if (!mFont.loadFromFile("font1.ttf"))
	{
		throw std::runtime_error("Unable to load font1.ttf");
	}
}

void GameOfLife::initZoomArea()
{
	float zoomAreaSize = sGridSize / mZoom.getFractionOfTheGrid();
	mZoomArea = sf::RectangleShape(sf::Vector2f(zoomAreaSize, zoomAreaSize));
	mZoomArea.setOrigin(sf::Vector2f(zoomAreaSize / 2, zoomAreaSize / 2));
	mZoomArea.setOutlineThickness(2);
	mZoomArea.setOutlineColor(sf::Color::Red);
	mZoomArea.setFillColor(sf::Color::Transparent);
}


void GameOfLife::resetInfos()
{
	mGeneration = 0;
	std::string title = sTitle + " : 0";
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
		while (timeSinceLastUpdate > sTimePerFrame)
		{
			timeSinceLastUpdate -= sTimePerFrame;
			if (mFileModule.hasMessage())
			{
				handleFileEvent();
			}
			else
			{
				handleEvent();
			}
			
			update(sTimePerFrame);
		}
		render();
	}
}

void GameOfLife::handleFileEvent()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{	
		if (mFileModule.isSaving() || mFileModule.isLoading())
		{
			if (event.type == sf::Event::TextEntered && event.text.unicode < 128)
			{
				char c = static_cast<char>(event.text.unicode);
				if (isalnum(c) || ispunct(c))
				{
					mBuffer += c;
				}			
				mFileModule.popMessage();
				mFileModule.printMessage("    " + mBuffer);
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::BackSpace:
					if (!mBuffer.empty())
					{
						mBuffer.pop_back();
					}
					break;
				case sf::Keyboard::Return:
					if (mFileModule.isSaving())
					{
						mFileModule.saveGrid(mCellsMatrix, mBuffer);
						mBuffer.clear();
					}
					else if (mFileModule.isLoading())
					{
						if (mFileModule.loadGrid(mCellsMatrix, mBuffer) && !mZoom.isActive())
						{
							updateVertices();
						}
						mBuffer.clear();
					}
					break;
				case sf::Keyboard::Escape:
					mFileModule.clearMessage();
					mBuffer.clear();
					break;
				case sf::Keyboard::Left:
					if (mFileModule.isLoading())
					{
						mFileModule.clearMessage();
						mFileModule.load(FileModule::Navigation::previous);
					}
					break;
				case sf::Keyboard::Right:
					if (mFileModule.isLoading())
					{
						mFileModule.clearMessage();
						mFileModule.load(FileModule::Navigation::next);
					}
					break;
				}
			}
			else if (event.type == sf::Event::Closed)
			{
				mWindow.close();
			}
		}
		else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonReleased)
		{
			mBuffer.clear();
			mFileModule.clearMessage();
		}

	}
}

void GameOfLife::handleEvent()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed)
		{
			handleKeyPressed(event.key.code);
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::C:
				if (mIsPaused)
				{
					mFileModule.load(FileModule::Navigation::first);
				}
				break;
			case sf::Keyboard::S:
				if (mIsPaused)
				{
					mFileModule.save();
				}
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
			else if (mIsPaused && event.mouseButton.button == sf::Mouse::Left)
			{
				sf::Vector2i pos = sf::Mouse::getPosition(mWindow);
				if (!mZoom.isActive())
				{
					if (pos.x < sGridSize && pos.y < sGridSize)
					{
						mCellsMatrix[pos.x + pos.y * sGridSize] = !mCellsMatrix[pos.x + pos.y * sGridSize];
						mGrid[pos.x + pos.y * sGridSize].color = mCellsMatrix[pos.x + pos.y * sGridSize] ? sAliveColor : sDeadColor;
					}
				}
				else
				{
					if (pos.x < sGridSize && pos.y < sGridSize)
					{
						sf::Vector2i coord = mZoom.updateGrid(pos);
						if (coord != sf::Vector2i(-1, -1))
						{
							mGrid[coord.x + coord.y * sGridSize].color = mCellsMatrix[coord.x + coord.y * sGridSize] ? sAliveColor : sDeadColor;
						}
					}
				}
			}
		}
		else if (event.type == sf::Event::MouseWheelMoved)
		{
			if (event.mouseWheel.delta < 0)
			{
				mZoom.reduceZoom();
				initZoomArea();
			}
			else
			{
				mZoom.augmentZoom();
				initZoomArea();
			}
		}
		else if (event.type == sf::Event::Closed)
		{
			mWindow.close();
		}
	}
}

void GameOfLife::handleKeyPressed(const sf::Keyboard::Key &key)
{
	switch (key)
	{
	case sf::Keyboard::Escape:
		mWindow.close();
		break;
	case sf::Keyboard::Return:
		mIsPaused = !mIsPaused;
		break;
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
	case sf::Keyboard::N:
		if (mIsPaused)
		{
			updateGrid();
			++mGeneration;
			std::string title = sTitle + " : " + std::to_string(mGeneration);
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
	case sf::Keyboard::M:
		if (mIsPaused)
		{
			mFileModule.magic(mCellsMatrix);
			if (!mZoom.isActive())
			{
				updateVertices();
			}
		}

		break;
	default:
		break;
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
			std::string title = sTitle + " : " + std::to_string(mGeneration);
			mWindow.setTitle(title);
			updateVertices();
		}
	}

	if (mIsPaused && !mZoom.isActive() && sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		sf::Vector2i pos = sf::Mouse::getPosition(mWindow);
		if (!mZoom.isActive())
		{
			if (pos.x < sGridSize && pos.y < sGridSize)
			{
				mCellsMatrix[pos.x + pos.y * sGridSize] = !mCellsMatrix[pos.x + pos.y * sGridSize];
				mGrid[pos.x + pos.y * sGridSize].color = mCellsMatrix[pos.x + pos.y * sGridSize] ? sAliveColor : sDeadColor;
			}
		}
		else
		{
			if (pos.x < sGridSize && pos.y < sGridSize)
			{
				sf::Vector2i coord = mZoom.updateGrid(pos);
				if (coord != sf::Vector2i(-1, -1))
				{
					mGrid[coord.x + coord.y * sGridSize].color = mCellsMatrix[coord.x + coord.y * sGridSize] ? sAliveColor : sDeadColor;
				}
			}
		}
	}

	sf::Vector2i mousePosition = sf::Mouse::getPosition(mWindow);
	mZoomArea.setPosition(static_cast<sf::Vector2f>(mousePosition));
}

void GameOfLife::render()
{
	mWindow.clear(sBackgroundColor);
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
	mMenu.draw(mWindow, mGeneration, mSpeed, mZoom.getFractionOfTheGrid(), mZoom.isActive(), mIsPaused);
	mFileModule.draw();
	mWindow.display();
}

void GameOfLife::updateGrid()
{
	int nb_neighbor;
	int indexWidthMax = sGridSize - 1;
	int indexHeightMax = sGridSize - 1;
	for (int h = 0; h < sGridSize; ++h)
	{
		for (int w = 0; w < sGridSize; ++w)
		{
			nb_neighbor = 0;
			//line above
			if (w > 0 && h > 0 && mCellsMatrix[w - 1 + sGridSize * (h - 1)])
				++nb_neighbor;
			if (h > 0 && mCellsMatrix[w + sGridSize * (h - 1)])
				++nb_neighbor;
			if (w < indexWidthMax && h > 0 && mCellsMatrix[w + 1 + sGridSize * (h - 1)])
				++nb_neighbor;
			//same line
			if (w > 0 && mCellsMatrix[w - 1 + sGridSize * h])
				++nb_neighbor;
			if (w < indexWidthMax && mCellsMatrix[w + 1 + sGridSize * h])
				++nb_neighbor;
			//line below
			if (w > 0 && h < indexHeightMax && mCellsMatrix[w - 1 + sGridSize * (h + 1)])
				++nb_neighbor;
			if (h < indexHeightMax && mCellsMatrix[w + sGridSize * (h + 1)])
				++nb_neighbor;
			if (w < indexWidthMax && h < indexHeightMax && mCellsMatrix[w + 1 + sGridSize * (h + 1)])
				++nb_neighbor;
			//cell update
			if (!mCellsMatrix[w + sGridSize * h] && nb_neighbor == 3)
				mTempCellsMatrix[w + sGridSize * h] = true;
			else if (mCellsMatrix[w + sGridSize * h] && (nb_neighbor < 2 || nb_neighbor > 3))
				mTempCellsMatrix[w + sGridSize * h] = false;
			else
				mTempCellsMatrix[w + sGridSize * h] = mCellsMatrix[w + sGridSize * h];
		}
	}
	mCellsMatrix.swap(mTempCellsMatrix);
}

void GameOfLife::updateVertices()
{
	int index;
	for (int h = 0; h < sGridSize; ++h)
	{
		for (int w = 0; w < sGridSize; ++w)
		{
			index = w + h * sGridSize;
			mGrid[index].color = mCellsMatrix[index] ? sAliveColor : sDeadColor;
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
		for (int h = 0; h < sGridSize; ++h)
		{
			for (int w = 0; w < sGridSize; ++w)
			{
				int alea = uniform_dist(e1);
				mCellsMatrix[w + h * sGridSize] = alea < ratio ? true : false;
			}
		}
		updateVertices();
	}
}

void GameOfLife::reverse_grid()
{
	assert(mIsPaused);
	for (int h = 0; h < sGridSize; ++h)
	{
		for (int w = 0; w < sGridSize; ++w)
		{
			mCellsMatrix[w + h * sGridSize] = !mCellsMatrix[w + h * sGridSize];
		}
	}
	updateVertices();
}

void GameOfLife::speedUpDt(bool accelerate)
{
	if (accelerate)
	{
		mSpeed = (mSpeed + 1) > hyperspeed ? hyperspeed : mSpeed + 1;
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
	case hyperspeed:
		mDelay = sf::seconds(1.f / 240.f);
		break;
	default:
		break;
	}
}
