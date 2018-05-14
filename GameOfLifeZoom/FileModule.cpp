#include "FileModule.h"
#include <iostream> //TODO

FileModule::FileModule(sf::RenderWindow & window, sf::Font &font)
	: mWindow(window)
	, mHasMessage(false)
	, mFont(font)
	, mIsSaving(false)
	, mIsLoading(false)
{}

bool FileModule::saveGrid(std::vector<bool>& cellsMatrix, const std::string &filename)
{
	std::ofstream outputFile;
	outputFile.open(filename, std::ios::binary);
	if (!outputFile.is_open())
	{
		printMessage(std::string("Sauvegarde de ") + filename + " a échoué.");
		return false;
	}
	int byte_index = 0;
	char byte = 0;
	for (size_t i = 0; i < cellsMatrix.size(); ++i)
	{
		if (cellsMatrix[i])
		{
			byte |= (1 << byte_index);
		}
		++byte_index;
		if (byte_index > 7)
		{
			byte_index = 0;
			outputFile.write(&byte, sizeof(byte));
			byte = 0;
		}
	}
	outputFile.close();
	printMessage(filename + " sauvegardé !");
	return true;
}

bool FileModule::loadGrid(std::vector<bool>& cellsMatrix, const std::string &filename)
{
	std::ifstream inputFile;
	inputFile.open(filename, std::ios::binary);
	if (!inputFile.is_open())
	{
		printMessage(std::string("Chargement de ") + filename + " a échoué.");
		return false;
	}
	size_t size = cellsMatrix.size() / 8;//nombre d'octects à lire : 45000
	char * buffer = new char[size];
	inputFile.read(buffer, size);
	inputFile.close();
	cellsMatrix.clear();
	for (size_t i = 0; i < size; ++i)
	{
		for (int bit_index = 0; bit_index < 8; ++bit_index)
		{
			cellsMatrix.emplace_back((static_cast<unsigned char>(buffer[i]) & (1U << bit_index)) >> bit_index);
		}
	}
	delete[] buffer;
	inputFile.close();
	printMessage(filename + " chargé !");
	return true;
}

void FileModule::save()
{
	mIsSaving = true;
}

void FileModule::load()
{
	mIsLoading = true;
}

bool FileModule::isSaving()
{
	return mIsSaving;
}

bool FileModule::isLoading()
{
	return mIsLoading;
}

void FileModule::draw(std::string &filenameBuffer)
{
	std::cout << filenameBuffer << std::endl;
	if (mHasMessage)
	{
		sf::Vector2f winSize(mWindow.getSize());
		sf::RectangleShape msgBackground(winSize);
		msgBackground.setFillColor(sf::Color(0, 0, 0, 128));
		mWindow.draw(msgBackground);

		sf::Text msgText;
		msgText.setFont(mFont);
		msgText.setCharacterSize(20);
		int xText = 100;
		int yText = 100;
		
		msgText.setFillColor(sf::Color::White);
		for (auto msg : mMessage)
		{
			msgText.setString(msg);
			yText += 30;
			msgText.setPosition(xText, yText);
			mWindow.draw(msgText);
		}
		msgText.setString("Appuyez sur une touche pour continuer...");
		yText += 30;
		msgText.setPosition(xText, yText);
		mWindow.draw(msgText);
	}
}

void FileModule::printMessage(std::string message)
{
	mMessage.push_back(message);
	mHasMessage = true;
}

void FileModule::clearMessage()
{
	mMessage.clear();
	mHasMessage = false;
}

bool FileModule::hasMessage()
{
	return mHasMessage;
}