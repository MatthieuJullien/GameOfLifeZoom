#pragma once
#include <string>
#include <vector>
#include <deque>
#include <fstream>
#include <SFML/Graphics.hpp>

class FileModule
{
public:

	enum Navigation
	{
		previous = -1,
		first = 0,
		next = 1
	};

	FileModule(sf::RenderWindow &window, sf::Font &font);
	bool saveGrid(std::vector<bool> &cellsMatrix, std::string &filename);
	bool loadGrid(std::vector<bool> &cellsMatrix, std::string &filename);
	void save();
	void load(const int page);
	bool isSaving() const;
	bool isLoading() const;
	bool hasMessage() const;
	void draw();
	void printMessage(std::string message);
	void popMessage();
	void clearMessage();
	void magic(std::vector<bool> &cellsMatrix);

private:
	sf::RenderWindow &mWindow;
	sf::Font &mFont;
	bool mHasMessage;
	std::deque<std::string> mFileNames;
	std::vector<std::string> mMessage;
	bool mIsSaving;
	bool mIsLoading;
	int mPage;
};

