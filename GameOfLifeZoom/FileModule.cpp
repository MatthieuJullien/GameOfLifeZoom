#include "FileModule.h"
#include <experimental/filesystem>
#include <iostream>//TODO
#include <random>
#include <cassert>

FileModule::FileModule(sf::RenderWindow & window, sf::Font &font)
	: mWindow(window)
	, mHasMessage(false)
	, mFont(font)
	, mFileNames()
	, mIsSaving(false)
	, mIsLoading(false)
	, mPage(0)
{
	namespace fs = std::experimental::filesystem;
	fs::directory_iterator it(fs::current_path());
	fs::directory_iterator end;
	for (auto entry : fs::directory_iterator(fs::current_path()))
	{
		if (entry.path().extension().string() == ".rle")
		{
			if (entry.path().stem().string().substr(0, 5) == "grid.")
			{
				mFileNames.push_front(entry.path().stem().string());
			}
			else
			{
				mFileNames.push_back(entry.path().stem().string());
			}
		}
	}
	for (auto f : mFileNames)
		std::cout << f << std::endl;
}

bool FileModule::saveGrid(std::vector<bool>& cellsMatrix, std::string &filename)
{
	std::ofstream outputFile; 
	std::string fullFilename = "grid." + filename + ".rle";
	outputFile.open(fullFilename, std::ios::binary);
	if (!outputFile.is_open())
	{
		clearMessage();
		printMessage(std::string("Sauvegarde de ") + fullFilename + " a échoué.");
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
	mFileNames.push_front("grid." + filename);
	clearMessage();
	printMessage(fullFilename + " sauvegardé !");
	printMessage("Appuyez sur une touche pour continuer...");
	mIsSaving = false;
	return true;
}

bool FileModule::loadGrid(std::vector<bool>& cellsMatrix, std::string &filename)
{
	std::ifstream inputFile;
	filename += ".rle";
	inputFile.open(filename, std::ios::binary);
	if (!inputFile.is_open())
	{
		clearMessage();
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
	clearMessage();
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

void FileModule::load(const int page)
{
	mIsLoading = true;
	printMessage("Liste des grilles enregistrées :");
	printMessage("");
	mPage += page;
	const int nbFilePerPage = 10;
	if (mPage * nbFilePerPage >= mFileNames.size())
	{
		--mPage;
	}
	if (mPage < 0)
	{
		mPage = 0;
	}
	for (int i = mPage * nbFilePerPage; i < mFileNames.size() && i < (mPage + 1) * nbFilePerPage; ++i)
	{
		std::cout << i << " : " << mFileNames[i] << std::endl;
		printMessage("*        " + mFileNames[i]);
	}
	printMessage("");
	printMessage("Page " + std::to_string(mPage));
	printMessage("Page suivante : flèche droite");
	printMessage("Entrez le nom de la grille à charger :");
	printMessage("");
}

bool FileModule::isSaving() const
{
	return mIsSaving;
}

bool FileModule::isLoading() const
{
	return mIsLoading;
}

bool FileModule::hasMessage() const
{
	return mHasMessage;
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
		int yText = 20;
		
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
	mIsSaving = false;
	mIsLoading = false;
}

void FileModule::magic(std::vector<bool>& cellsMatrix)
{
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(0, mFileNames.size() - 1);
	int alea = uniform_dist(e1);
	std::string filename = mFileNames[alea];
	bool isLoaded = FileModule::loadGrid(cellsMatrix, filename);
	assert(isLoaded);
}