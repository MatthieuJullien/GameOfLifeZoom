#include "Menu.h"

Menu::Menu(sf::Font & font)
	: mFont(font)
{}

void Menu::draw(sf::RenderWindow & window, int generation, int speed, int zoomLevel, bool isZoom, bool isPaused)
{
	sf::Text menu;
	std::string menuString;
	std::string pause;
	std::string suivant;
	std::string zoom;
	std::string zoomStr;
	std::string clicGauche;
	std::string populate;
	std::string lecture;
	if (isPaused)
	{
		pause = "    Lecture\n\n";
		suivant = "N :\n    Génération suivante\n\n";
		clicGauche = "Clic gauche :\n    Editer la grille\n\n";
		populate = "0 à 9 :\n    Redémarre et remplie la grille aléatoirement\n    (0 à 90 % de cellules vivantes)\n\n";
		lecture = "Off";
	}
	else
	{
		pause = "    Pause\n\n";
		suivant = "";
		clicGauche = "";
		populate = "";
		lecture = "On";
	}
	if (isZoom)
	{
		zoom = "    Dézoomer\n\n";
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
		+ "Niveau de zoom : " + std::to_string(zoomLevel - 1) + "/ 15\n"
		+ "Zoom : " + zoomStr + "\n"
		+ "Lecture : " + lecture + "\n"
		+ "-----------------------------------------------------\n"
		+ "Echap :\n"
		+ "    Quitter\n\n"
		+ "Entree, Space :\n"
		+ pause
		+ "Clic droit : "
		+ zoom
		+ "- / + :\n"
		+ "    Modifier vitesse\n\n"
		+ "P :\n"
		+ "    Augmente le niveau de zoom\n"
		+ "M :\n"
		+ "    Diminue le niveau de zoom\n\n"
		+ "I :\n"
		+ "    Inverser l'état des cellules\n\n"
		+ "S :\n"
		+ "    Sauvegarder la grille\n"
		+ "C :\n"
		+ "    Charger une grille\n\n"
		+ populate
		+ clicGauche
		+ suivant;

	menu.setFont(mFont);
	menu.setString(menuString);
	menu.setCharacterSize(12);
	menu.setFillColor(sf::Color::Blue);
	menu.setPosition(620, 5);
	window.draw(menu);
}
