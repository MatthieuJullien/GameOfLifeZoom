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
	std::string magic;
	std::string clicGauche;
	std::string populate;
	std::string lecture;
	if (isPaused)
	{
		pause = "    Lecture\n\n";
		suivant = "N :\n    G�n�ration suivante\n\n";
		magic = "M :\n    Touche magique\n\n";
		clicGauche = "Clic gauche :\n    Editer la grille\n\n";
		populate = "0 � 9 :\n    Red�marre et remplie la grille al�atoirement\n    (0 � 90 % de cellules vivantes)\n\n";
		lecture = "Off";
	}
	else
	{
		pause = "    Pause\n\n";
		suivant = "";
		magic = "";
		clicGauche = "";
		populate = "";
		lecture = "On";
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
		+ "Vitesse de la simulation : " + std::to_string(speed + 1) + "/8\n"
		+ "Niveau de zoom : " + std::to_string(zoomLevel - 1) + "/ 15\n"
		+ "Zoom : " + zoomStr + "\n"
		+ "Lecture : " + lecture + "\n"
		+ "-------------------------------------\n"
		+ "Echap :\n"
		+ "    Quitter\n\n"
		+ "Entree, Space :\n"
		+ pause
		+ "Clic droit : "
		+ zoom
		+ "- / + :\n"
		+ "    Modifier vitesse\n\n"
		+ "Molette souris bas / haut :\n"
		+ "    Modifier le niveau de zoom\n"
		+ "I :\n"
		+ "    Inverser l'�tat des cellules\n\n"
		+ "S :\n"
		+ "    Sauvegarder la grille\n"
		+ "C :\n"
		+ "    Charger une grille\n"
		+ magic
		+ populate
		+ clicGauche
		+ suivant;

	menu.setFont(mFont);
	menu.setString(menuString);
	menu.setCharacterSize(13);
	menu.setFillColor(sf::Color::Blue);
	menu.setPosition(620, 5);
	window.draw(menu);
}
