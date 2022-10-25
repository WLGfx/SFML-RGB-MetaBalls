/*
 * MetaBalls.h
 *
 *  Created on: 23 Oct 2022
 *      Author: wlgfx
 */

#ifndef METABALLS_H_
#define METABALLS_H_

#include <stdlib.h>
#include <vector>
#include <thread>
#include <SFML/Graphics.hpp>

#define BALLS_MAX 12
#define THREADS_MAX 4

struct MetaBall {
	sf::Vector2f pos;
	sf::Vector2f dir;
	float speed;
	float radius;
	sf::Color color;
};

struct ThreadDrawData {
	sf::Rect<uint> bounds;
	MetaBall *balls;
	sf::Color *cols;
	sf::Image *image;
	bool ready;
	bool quit = false;
};

class MetaBalls {
public:
	MetaBalls(sf::RenderWindow *parentWindow);
	virtual ~MetaBalls();

	void update();
	void draw();
	void quit();

private:
	sf::RenderWindow *window;
	sf::Vector2u windowSize;
	MetaBall balls[BALLS_MAX];
	sf::Clock clock;
	float delta = 0.0f;
	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;
	sf::CircleShape drawable;

	sf::Color cols[6] = {
			sf::Color::Blue,
			sf::Color::Cyan,
			sf::Color::Green,
			sf::Color::Magenta,
			sf::Color::Red,
			sf::Color::Yellow
	};

	void init();
	uint randi(uint max) { return rand() % max; }
	float randf() { return float(rand()) / float(RAND_MAX); }
	float randf1() { return float(rand()) / float(RAND_MAX >> 1) - 1.0f; }
	float randfm(float max) { return (float(rand()) / float(RAND_MAX)) * max; }
	sf::Vector2f normalizef(sf::Vector2f &vec);

	void move_balls();
	//void draw_image();
	void draw_balls();

	//sf::ThreadLocalPtr<ThreadDrawData> objPtr;
	ThreadDrawData threadData[THREADS_MAX];
	std::thread drawThread[THREADS_MAX];

public:
	static void threadDrawDataRun(ThreadDrawData *data);
};

#endif /* METABALLS_H_ */
