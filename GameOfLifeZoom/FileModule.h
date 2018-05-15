#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <SFML/Graphics.hpp>

class FileModule
{
public:
	FileModule(sf::RenderWindow &window, sf::Font &font);
	bool saveGrid(std::vector<bool> &cellsMatrix, std::string &filename);
	bool loadGrid(std::vector<bool> &cellsMatrix, std::string &filename);
	void save();
	void load();
	bool isSaving();
	bool isLoading();
	void draw();
	void printMessage(std::string message);
	void popMessage();
	void clearMessage();
	bool hasMessage();

private:
	sf::RenderWindow &mWindow;
	sf::Font &mFont;
	bool mHasMessage;
	std::vector<std::string> mMessage;
	bool mIsSaving;
	bool mIsLoading;
};

