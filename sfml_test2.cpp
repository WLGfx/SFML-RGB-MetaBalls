//============================================================================
// Name        : sfml_test2.cpp
// Author      : Carl Norwood
// Version     :
// Copyright   : WLGfx 2022
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <SFML/Graphics.hpp>

#include "MetaBalls.h"

int main() {
	sf::RenderWindow window(sf::VideoMode(1024, 768), "WLGfx");

	MetaBalls meta(&window);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				meta.quit();
				window.close();
			}
		}

		window.clear();

		meta.update();
		meta.draw();

		window.display();
	}
	return 0;
}
