#include "FileModule.h"
#include <experimental/filesystem>
#include <iostream> //TODO

FileModule::FileModule(sf::RenderWindow & window, sf::Font &font)
	: mWindow(window)
	, mHasMessage(false)
	, mFont(font)
	, mIsSaving(false)
	, mIsLoading(false)
{}

bool FileModule::saveGrid(std::vector<bool>& cellsMatrix, std::string &filename)
{
	std::ofstream outputFile; 
	filename += ".grid";
	outputFile.open(filename, std::ios::binary);
	if (!outputFile.is_open())
	{
		printMessage(std::string("Sauvegarde de ") + filename + " a échoué.");
		printMessage("Appuyez sur une touche pour continuer...");
		mIsSaving = false;
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
	printMessage("Appuyez sur une touche pour continuer...");
	mIsSaving = false;
	return true;
}

bool FileModule::loadGrid(std::vector<bool>& cellsMatrix, std::string &filename)
{
	std::ifstream inputFile;
	filename += ".grid";
	inputFile.open(filename, std::ios::binary);
	if (!inputFile.is_open())
	{
		printMessage(std::string("Chargement de ") + filename + " a échoué.");
		printMessage("Appuyez sur une touche pour continuer...");
		mIsLoading = false;
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
	printMessage("Appuyez sur une touche pour continuer...");
	mIsLoading = false;
	return true;
}

void FileModule::save()
{
	mIsSaving = true;
	printMessage("Entrez le nom de la grille :");
	printMessage("");
}

void FileModule::load()
{
	namespace fs = std::experimental::filesystem;

	mIsLoading = true;
	printMessage("Liste des grilles enregistrées :");
	printMessage("");
	for (auto& p : fs::directory_iterator(fs::current_path()))
	{
		if (p.path().extension().string() == ".grid")
		{
			printMessage("*        " + p.path().stem().string());
		}
	}
	printMessage("");
	printMessage("Entrez le nom de la grille à charger :");
	printMessage("");
}

bool FileModule::isSaving()
{
	return mIsSaving;
}

bool FileModule::isLoading()
{
	return mIsLoading;
}

void FileModule::draw()
{
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
		mWindow.draw(msgText);
	}
}

void FileModule::printMessage(std::string message)
{
	mMessage.push_back(message);
	mHasMessage = true;
}

void FileModule::popMessage()
{
	if (!mMessage.empty())
	{
		mMessage.pop_back();
	}
	if (mMessage.empty())
	{
		mHasMessage = false;
	}
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