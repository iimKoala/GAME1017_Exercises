#pragma once
#ifndef _ENGINE_H_ // Portable. Oldschool. Better standard.
#define _ENGINE_H_

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h> 
#include "Missile.h"
#include "Player.h"
#include "Enemy.h"
#include "Timer.h"
#include <vector> 
#define FPS 60
#define WIDTH 1024
#define HEIGHT 768
#define SPEED 5

using namespace std;


class Engine
{
private:
	SDL_Renderer* g_pRenderer;
	
	
public:
	int Init(const char* title, int xPos, int yPos, int width, int height, int flags);
	void HandleEvents();
	bool KeyDown(SDL_Scancode c);
	void Update();
	void Render();
	void Clean();
	void Wake();
	void Sleep();
	static Engine& Instance();
	SDL_Renderer* GetRenderer() { return g_pRenderer; }
	
	int Run();
	bool g_running = false;
	Player g_player=(WIDTH / 2, HEIGHT / 2);
	Timer timerSpawnEnemy=(3000); // Timer for spawn Enemies
	Timer timerSpawnEnemyMissile=(2000); // Timer for spawn Enemies Missiles
	Uint32 g_start, g_end, g_delta, g_fps;
	const Uint8* g_keystates;
	SDL_Window* g_pWindow;
	SDL_Texture* g_pShipTexture; // Every source image needs a SDL_Texture*.
	SDL_Texture* g_pPlayerShipTexture; // Every source image needs a SDL_Texture*.

	SDL_Texture* g_pBGTexture; // For background.
	SDL_Texture* g_pEnemyTexture; // For Week 11.
	SDL_Texture* g_pEnemy2Texture; // For Week 11.
	SDL_Texture* g_pPlayerMissTexture; // For Week 11.
	SDL_Texture* g_pEnemyMissTexture; // For Week 11.

	SDL_Rect g_playerx; // For primitive rectangle.
	SDL_Rect g_src, g_dst; // For the ship sprite.
	SDL_Rect g_space = { 0,0,1024,768 };
	// No source or destination rectangles for bg, because we'll take all pixels.
	// Two destination rectangles for BG
	SDL_Rect g_BG1, g_BG2;
	bool g_fire = false; // Fire toggle.
	bool g_turret = false; // Another fire toggle for turret.
	SDL_Point g_mousePos;

	

	vector<Enemy*> g_Enemy;
	vector<Missile*> g_playerMissiles;
	vector<Missile*> g_enemyMissiles;
	

	int kills = 0, hits = 0;

	// Week 14 stuff.
	Mix_Chunk* g_pBoom;
	Mix_Chunk* g_pMissile; // death.wav
	Mix_Chunk* g_pMissileE;
	Mix_Chunk* g_pLaser;
	Mix_Chunk* g_pExplosion;

	Mix_Music* g_pTcats;
	Mix_Music* g_pTmnt ;
	bool g_paused = 0; // For toggle.
	bool Alive = true;
};
#endif