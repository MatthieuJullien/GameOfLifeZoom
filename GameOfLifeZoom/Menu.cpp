#include "Menu.h"

void Menu::draw(sf::RenderWindow & window, int generation, int speed, int zoomLevel, bool isZoom, bool isPaused)
{
	sf::Font font;
	if (!font.loadFromFile("font1.ttf"))
	{
		throw std::runtime_error("Unable to load font1.ttf");
	}
	sf::Text menu;
	std::string menuString;
	std::string pause;
	std::string suivant;
	std::string zoom;
	std::string zoomStr;
	std::string clicGauche;
	if (isPaused)
	{
		pause = "    Lecture\n\n";
		suivant = "S :\n    G�n�ration suivante\n\n";
		clicGauche = "Clic gauche :\n    Editer la grille\n\n";
	}
	else
	{
		pause = "    Pause\n\n";
		suivant = "";
		clicGauche = "";
	}
	if (isZoom)
	{
		zoom = "    D�zoomer\n\n";
		zoomStr = "On";
	}
	else
	{
		zoom = "    Zoomer\n\n";
		zoomStr = "Off";
	}
	menuString = menuString
		+ "Generation : " + std::to_string(generation) + "\n"
		+ "Vitesse de la simulation : " + std::to_string(speed + 1) + "/7\n"
		+ "Niveau de zoom : " + std::to_string(zoomLevel) + "\n"
		+ "Zoom : " + zoomStr + "\n\n"
		+ "Echap :\n"
		+ "    Quitter\n\n"
		+ "Entree, Space :\n"
		+ pause
		+ "- / + :\n"
		+ "    Modifier vitesse\n\n"
		+ "P :\n"
		+ "    Red�marre et augmente le niveau de zoom\n"
		+ "M :\n"
		+ "    Red�marre et diminue le niveau de zoom\n"
		+ "0 � 9 :\n"
		+ "    Red�marre et remplie la grille al�atoirement\n"
		+ "    (0 � 90 % de cellules vivantes)\n\n"
		+ "R :\n"
		+ "    Inverser l'�tat des cellules\n\n"
		+ "Clic droit"
		+ zoom
		+ clicGauche
		+ suivant;

	menu.setFont(font);
	menu.setString(menuString);
	menu.setCharacterSize(14);
	menu.setFillColor(sf::Color::Blue);
	menu.setPosition(620, 10);
	window.draw(menu);
}
