// #pragma once // Microsoft or .NET standard. Not that portable.
#ifndef _MISSILE_H_ // Portable. Oldschool. Better standard.
#define _MISSILE_H_
#include <SDL.h>
#define MOVESPEED 10

enum stateMissile { PLAYER, ENEMY };

class Missile
{
private:
	unsigned short m_frameCtr,  // Counts the number of frames. 
		m_frameMax,	// Number of frames each sprite is displayed. Animation speed.
		m_spriteIdx, // Which sprite to display.
		m_spriteMax; // Number of sprites in the animation.
public:
	stateMissile state;
	SDL_Rect m_src, m_dst;
	Missile(int = 0, int = 0, stateMissile = ENEMY); // Default and non-default constructor.
	void Update();
};

#endif