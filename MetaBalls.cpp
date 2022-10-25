/*
 * MetaBalls.cpp
 *
 *  Created on: 23 Oct 2022
 *      Author: wlgfx
 */

#include <time.h>
#include <math.h>
#include <unistd.h>

#include "MetaBalls.h"

MetaBalls::MetaBalls(sf::RenderWindow *parentWindow) {
	window = parentWindow;
	init();
}

MetaBalls::~MetaBalls() {}

void MetaBalls::init() {
	srand(time(0)); // init RNG

	windowSize = window->getSize();

	for (MetaBall &ball : balls) {
		ball.pos.x = randfm(windowSize.x);
		ball.pos.y = randfm(windowSize.y);
		ball.dir = sf::Vector2f(randf1(), randf1());
		ball.dir = normalizef(ball.dir);
		ball.speed = 120;
		ball.color = sf::Color::Yellow;
		ball.radius = 300.0f;
	}

	// set the colors
	for (int c = 0; c < BALLS_MAX; c++) {
		balls[c].color = cols[c % 6];
	}

	printf("Window size: %d, %d\n", windowSize.x, windowSize.y);

	image.create(windowSize.x, windowSize.y, sf::Color::Black);
	texture.create(windowSize.x, windowSize.y);
	texture.setSmooth(true);
	texture.setSrgb(true);
	sprite.setTexture(texture);

	drawable.setRadius(8);
	drawable.setOrigin(8, 8);

	uint ygap = windowSize.y / THREADS_MAX;
	for (int pos = 0; pos < THREADS_MAX; pos++) {
		sf::Rect<uint> bounds = sf::Rect<uint>(
				sf::Vector2u(0, ygap * pos),
				sf::Vector2u(windowSize.x, ygap));
		threadData[pos].bounds = bounds;
		threadData[pos].balls = balls;
		threadData[pos].cols = cols;
		threadData[pos].image = &image;
		threadData[pos].ready = false;

		drawThread[pos] = std::thread(&MetaBalls::threadDrawDataRun, &threadData[pos]);
		printf("Thread %d created\n", pos);
	}
}

void MetaBalls::quit() {
	for (int pos = 0; pos < THREADS_MAX; pos++) {
		threadData[pos].quit = true;
		drawThread[pos].join();
	}
}

sf::Vector2f MetaBalls::normalizef(sf::Vector2f &vec) {
	float length = sqrt((vec.x * vec.x) + (vec.y * vec.y));
	if (length != 0) {
		return sf::Vector2f(vec.x / length, vec.y / length);
	}
	return vec;
}

void MetaBalls::update() {
	move_balls();
	//draw_image();
	for (ThreadDrawData &data : threadData) {
		data.ready = true;
	}

	bool check;
	do {
		usleep(1000);
		check = false;
		for (ThreadDrawData &data : threadData) {
			if (data.ready == false) {
				check = true;
				break;
			}
		}
	} while (check == false);
}

void MetaBalls::draw() {
	texture.loadFromImage(image);
	window->draw(sprite);

	draw_balls();

	delta = clock.restart().asSeconds(); // new delta time
}

void MetaBalls::move_balls() {
	for (MetaBall &ball : balls) {
		// get new offset using delta time
		float xdir = ball.dir.x * ball.speed * delta;
		float ydir = ball.dir.y * ball.speed * delta;
		// if ball will hit edges then reverse direction on the axis
		if (ball.pos.x + xdir < 0 || ball.pos.x >= windowSize.x) {
			xdir = -xdir;
			ball.dir.x = -ball.dir.x;
		}
		if (ball.pos.y + ydir < 0 || ball.pos.y >= windowSize.y) {
			ydir = -ydir;
			ball.dir.y = -ball.dir.y;
		}
		// update position
		ball.pos.x += xdir;
		ball.pos.y += ydir;
	}
}

// This was the single core version
/*void MetaBalls::draw_image() {
	int range = 512;

	// go through every pixel
	for (uint y = 0; y < windowSize.y; y++) {
		for (uint x = 0; x < windowSize.x; x++) {
			float r = 0, g = 0, b = 0, v = 0;

			for (MetaBall &ball : balls) {
				// get distance from point to ball
				float xd = (ball.pos.x - x);
				float yd = (ball.pos.y - y);
				float dist = (xd * xd + yd * yd);

				float val = (ball.radius * ball.radius) / dist;// * ball.radius;
				r += val * ball.color.r;
				g += val * ball.color.g;
				b += val * ball.color.b;
				v += val;
			}

			r /= BALLS_MAX;
			g /= BALLS_MAX;
			b /= BALLS_MAX;
			v /= BALLS_MAX;
			v *= 150.0f;

			if (r > range) r = range;
			if (g > range) g = range;
			if (b > range) b = range;
			if (v > range) v = range;

			// try one of these

			image.setPixel(x, y, sf::Color(r, g, b));
			//image.setPixel(x, y, sf::Color(v, v, v));
		}
	}
}*/

void MetaBalls::draw_balls() {
	for (MetaBall &ball : balls) {
		drawable.setPosition(ball.pos);
		drawable.setFillColor(ball.color);
		window->draw(drawable);
	}
}

void MetaBalls::threadDrawDataRun(ThreadDrawData *data) {
	printf("Thread started...\n");

	while (!data->quit) {
		usleep(1000);
		if (data->ready) {
			// *************************************************

			int range = 512;

			// go through every pixel
			for (uint y = data->bounds.top; y < data->bounds.top + data->bounds.height; y++) {
				for (uint x = data->bounds.left; x < data->bounds.left + data->bounds.width; x++) {
					float r = 0, g = 0, b = 0, v = 0;

					for (int pos = 0; pos < BALLS_MAX; pos++) {
						MetaBall *ball = data->balls + pos;
						// get distance from point to ball
						float xd = (ball->pos.x - x);
						float yd = (ball->pos.y - y);
						float dist = (xd * xd + yd * yd);

						float val = (ball->radius * ball->radius) / dist;// * ball.radius;
						r += val * ball->color.r;
						g += val * ball->color.g;
						b += val * ball->color.b;
						v += val;
					}

					r /= BALLS_MAX;
					g /= BALLS_MAX;
					b /= BALLS_MAX;
					v /= BALLS_MAX;
					v *= 150.0f;

					if (r > range) r = range;
					if (g > range) g = range;
					if (b > range) b = range;
					if (v > range) v = range;

					// try one of these

					//data->image->setPixel(x, y, sf::Color(r, g, b));
					data->image->setPixel(x, y, sf::Color(v, v, v));
				}
			}
			// *************************************************
		}
		data->ready = false;
	}
}
