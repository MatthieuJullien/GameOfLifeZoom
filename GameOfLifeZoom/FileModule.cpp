#include "FileModule.h"
#include "GameOfLife.h"
#include <experimental/filesystem>
#include <random>
#include <cassert>
#include <regex>

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
	fs::path rledir(fs::current_path().string() + "/rle/");
	fs::directory_iterator it(rledir);
	fs::directory_iterator end;
	for (auto entry : fs::directory_iterator(rledir))
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
}

bool FileModule::saveGrid(std::vector<bool>& cellsMatrix, std::string &filename)
{
	std::ofstream outputFile; 
	std::string fullFilename = "grid." + filename + ".rle";
	outputFile.open("rle/" + fullFilename, std::ios::binary);
	if (!outputFile.is_open())
	{
		clearMessage();
		printMessage("Sauvegarde de " + fullFilename + " a échoué.");
		printMessage("Appuyez sur une touche pour continuer...");
		mIsSaving = false;
		return false;
	}
	const int gridSize = GameOfLife::sGridSize;
	outputFile << "x = " << gridSize << ", y = " << gridSize << ", rule = B3/S23\n";
	bool curState;
	for (int h = 0; h < gridSize; ++h)
	{	
		curState = cellsMatrix[h * gridSize];
		int runCount = 0;
		for (int w = 0; w <= gridSize; ++w)
		{
			if (w == gridSize || cellsMatrix[w + h * gridSize] != curState)
			{
				char tag = curState ? 'o' : 'b';
				if (runCount == 1)
				{
					outputFile << tag;
				}
				else
				{
					outputFile << runCount << tag;
				}
				runCount = 1;
				curState = !curState;
			}
			else
			{
				++runCount;
			}
		}
		if (h < gridSize - 1)
		{
			outputFile << '$';
		}
	}
	outputFile << '!';
	
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
	inputFile.open("rle/" + filename);
	if (!inputFile.is_open())
	{
		clearMessage();
		printMessage("Chargement de " + filename + " a échoué.");
		printMessage("Appuyez sur une touche pour continuer...");
		mIsLoading = false;
		return false;
	}
	cellsMatrix.clear();
	std::string line;
	while (std::getline(inputFile, line) && line.at(0) == '#')
		;	//skip commentary 
	int x = std::stoi(std::regex_replace(line, std::regex("[^0-9]*([0-9]+).*"), std::string("$1")));
	int y = std::stoi(std::regex_replace(
		line,
		std::regex("[^0-9]*[0-9]+[^0-9]+([0-9]+).*"),
		std::string("$1")));
	if (x > GameOfLife::sGridSize || y > GameOfLife::sGridSize)
	{
		clearMessage();
		printMessage("Chargement de " + filename + " a échoué.");
		printMessage("Le pattern est trop grand pour la grille.");
		printMessage("Appuyez sur une touche pour continuer...");
		mIsLoading = false;
		return false;
	}
	int offsetX = (GameOfLife::sGridSize - x) / 2;
	int offsetY = (GameOfLife::sGridSize - y) / 2;
	for (int i = offsetX; i > 0; --i)
	{
		for (int j = 0; j < GameOfLife::sGridSize; ++j)
		{
			cellsMatrix.push_back(false);
		}
	}
	char c;
	std::string run_count;
	int total_line = 0;
	int line_count = 0;
	bool isNewLine = true;
	while (true)
	{
		c = '\0';
		run_count = "0";
		if (total_line == 0)	//new line
		{
			for (int i = 0; i < offsetY; ++i)
			{
				cellsMatrix.push_back(false);
			}
			total_line += offsetY;
		}
		while (inputFile.get(c) && (isdigit(c) || c == '\n'))
		{
			if (isdigit(c))
			{
				run_count += c;
			}
		}
		if (c != '$' && c != '!')
		{
			
			int rc = std::stoi(run_count);
			rc = (rc == 0) ? 1 : rc;
			total_line += rc;
			for (; rc > 0; --rc)
			{
				cellsMatrix.push_back(c == 'o');
			}
		}
		else
		{
			// Compléter la ligne avec des cellules mortes
			for (int i = GameOfLife::sGridSize - total_line; i > 0; --i)
			{
				cellsMatrix.push_back(false);
			}
			total_line = 0;
			if (c == '$')
			{
				++line_count;
			}
			if (c == '!')
			{
				++line_count;
				// Compléter la grille avec des lignes mortes
				for (int i = GameOfLife::sGridSize - (line_count + offsetX); i > 0; --i)
				{
					for (int j = 0; j < GameOfLife::sGridSize; ++j)
					{
						cellsMatrix.push_back(false);
					}
				}
				break;
			}
		}
	}

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
	printMessage("Le nom de la grille sera préfixé par 'grid.'");
	printMessage("La grille sera enregistrée dans un fichier .rle");
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