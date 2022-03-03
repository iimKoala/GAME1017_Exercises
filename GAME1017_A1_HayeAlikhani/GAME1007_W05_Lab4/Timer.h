//#pragma once
#ifndef _Timer_H_ // Portable. Oldschool. Better standard.
#define _Timer_H_
#include <SDL.h>
#include <iostream>
using namespace std;

class Timer
{
private:
	Uint32 g_start, g_end, g_delta, g_fps;
	bool spawn = false;
public:
	Timer(int t) {
		g_start = t;
		g_end = t + SDL_GetTicks();
	}
	~Timer() {}
	void update() {
		g_delta = SDL_GetTicks();
		if (g_delta >= g_end) {
			//cout << "booom" << endl;
			g_end = g_start + SDL_GetTicks();
			spawn = true;
		}
	}
	void resetSpawn()
	{
		spawn = false;
	}
	bool getSpawn() {
		return spawn;
	}
};

#endif