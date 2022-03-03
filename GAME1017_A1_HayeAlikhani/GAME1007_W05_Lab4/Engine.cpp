#include "Engine.h"
#include "States.h"
#include "StateManager.h"





// Init function. Sets up SDL and all subsystems and other (dynamic) memory allocation.
int Engine::Init(const char* title, int xPos, int yPos, int width, int height, int flags)
{
	cout << "Initializing game..." << endl;
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) // If initialization is okay.
	{
		// Try to create the SDL_Window.
		g_pWindow = SDL_CreateWindow(title, xPos, yPos, width, height, flags);
		if (g_pWindow != nullptr) // If window creation passes.
		{
			// Try to create the SDL_Renderer. (Back buffer)
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
			if (g_pRenderer != nullptr)
			{
				if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != 0)
				{
					g_pShipTexture = IMG_LoadTexture(g_pRenderer, "ship.png");
					g_pPlayerShipTexture = IMG_LoadTexture(g_pRenderer, "PlayerShip.png");
					g_pEnemy2Texture = IMG_LoadTexture(g_pRenderer, "EnemyShip.png");
					g_pEnemyTexture = IMG_LoadTexture(g_pRenderer, "Enemies.png");
					g_pPlayerMissTexture = IMG_LoadTexture(g_pRenderer, "PlayerMissile.png");
					g_pEnemyMissTexture = IMG_LoadTexture(g_pRenderer, "EnemyMissile.png");
					g_pBGTexture = IMG_LoadTexture(g_pRenderer, "BackGroundx.png");
					// Add check of texture pointer later...
				}
				else return false; // Image init failed.

				if (Mix_Init(MIX_INIT_MP3) != 0)
				{
					// Configure mixer.
					Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096); // If any lag in playing sounds, lower to 2048.
					Mix_AllocateChannels(16);
					// Load sounds.
					g_pBoom = Mix_LoadWAV("aud/boom.wav");
					//g_pMissile = Mix_LoadWAV("aud/death.wav");
					g_pMissile = Mix_LoadWAV("aud/GunShot5.wav");
					g_pMissileE = Mix_LoadWAV("aud/GunShot2.wav");
					g_pLaser = Mix_LoadWAV("aud/laser.wav");
					//g_pExplosion = Mix_LoadWAV("aud/Explosion.wav");
					g_pExplosion = Mix_LoadWAV("aud/GunShot4.wav");
					//g_pTcats = Mix_LoadMUS("aud/Thundercats.mp3");
					g_pTcats = Mix_LoadMUS("aud/BackGround1.mp3");
					g_pTmnt = Mix_LoadMUS("aud/Turtles.mp3");
				}
				else return false; // Mixer init failed.
			}
			else return false; // Renderer creation failed.
		}
		else return false; // Window creation failed.
	}
	else return false; // Initialization has failed.
	// If everything is okay, we are here...
	g_fps = (Uint32)round(1 / (double)FPS * 1000);
	g_keystates = SDL_GetKeyboardState(nullptr);
	g_playerx = { WIDTH / 2, HEIGHT / 2, 35, 55 };

	g_src = { 0, 0, 154, 221 }; // Clips out entire image.
	g_dst = { WIDTH / 2, HEIGHT / 2, 154, 221 }; // On screen location/appearance.

	g_BG1 = { 0, 0, 1024, 768 };
	g_BG2 = { 0, -768, 1024, 768 };

	//g_missiles.reserve(4); // Pre-allocates 4 elements of the vector array. Capacity = 4.
	g_playerMissiles.reserve(4);
	g_Enemy.reserve(4);
	g_enemyMissiles.reserve(4);

	Mix_VolumeMusic(16); // 0-128.
	Mix_Volume(-1, 32); // All sfx.
	Mix_PlayMusic(g_pTcats, -1);
	STMA::ChangeState(new TitleState());
	cout << "Initialization successful!" << endl;
	g_running = true;
	return true;
}

// HandleEvents function. Gets input from user, e.g. mouse/keyboard/gamepad events.
void Engine::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: // Pressing 'X' button of window.
			g_running = false;
			break;
		case SDL_KEYDOWN: // Essential parses every frame. Just like keystates.
			if (event.key.keysym.sym == SDLK_SPACE) // Pressing spacebar.
			{
				g_fire = true;
				if (!Mix_Playing(8))
				{
					Mix_SetPanning(8, 255, 128);
					Mix_PlayChannel(8, g_pLaser, -1);
				}
				// cout << "Firin' mah lazor!" << endl;
			}
			break;
		case SDL_KEYUP: // One-shot
			if (event.key.keysym.sym == ' ') // Releasing spacebar.
			{
				g_fire = false;
				Mix_HaltChannel(8);
				cout << "Done pews..." << endl;
			}
			else if (event.key.keysym.sym == 13) // Enter.
			{
				// Fire dynamic Missile.
				//g_missiles.push_back(new Missile(g_dst.x + 68, g_dst.y));  // Instead of 68 we could add (g_dst.w/2).
				//g_missiles.shrink_to_fit();
				g_playerMissiles.push_back(new Missile(g_dst.x + (g_dst.w / 2) - 20, g_dst.y, PLAYER));  // Instead of 68 we could add (g_dst.w/2).
				g_playerMissiles.shrink_to_fit();
				Mix_PlayChannel(-1, g_pMissile, 0);
			}
			else if (event.key.keysym.sym == SDLK_1)
			{
				Mix_FadeOutMusic(1000);
				Mix_FadeInMusic(g_pTmnt, -1, 1000);
			}
			else if (event.key.keysym.sym == SDLK_2)
			{
				Mix_FadeOutMusic(1000);
				Mix_FadeInMusic(g_pTcats, -1, 1000);
			}
			else if (event.key.keysym.sym == SDLK_p) // Pause/resume toggle.
			{
				if (g_paused)
				{
					Mix_ResumeMusic();
					g_paused = false;
				}
				else
				{
					Mix_PauseMusic();
					Mix_HaltChannel(-1); // Optional. Also stops all sfx.
					g_paused = true;
				}
			}
			break;
		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&g_mousePos.x, &g_mousePos.y);
			// cout << '(' << g_mousePos.x << ',' << g_mousePos.y << ')' << endl;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
				g_turret = true;
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
				g_turret = false;
			break;
		}
	}
}

bool Engine::KeyDown(SDL_Scancode c)
{
	if (g_keystates != nullptr)
	{
		if (g_keystates[c] == 1) // Key we're testing for is down.
			return true;
	}
	return false;
}

// Update function. Moves objects, performs physics, e.g. projectiles, gravity, collisions.
void Engine::Update()
{
	STMA::Update();
}

// Render function. Renders changes in game objects to window.
void Engine::Render()
{
	STMA::Render();
}


Engine& Engine::Instance() // No static keyword required.
{
	static Engine instance; // Creating an object of Engine. Magic statics.
	return instance;
}
// Clean function. De-initialize SDL and de-allocate memory.
void Engine::Clean()
{
	cout << "Cleaning up..." << endl;
	
	for (unsigned i = 0; i < g_playerMissiles.size(); i++)
	{
		delete g_playerMissiles[i]; // Deallocates Missile through pointer.
		g_playerMissiles[i] = nullptr; // Ensures no dangling pointer.
	}
	g_playerMissiles.clear(); // Removes all elements. Size = 0.
	g_playerMissiles.shrink_to_fit(); // Sets capacity to size.

	// Clean up vector.
	for (unsigned i = 0; i < g_Enemy.size(); i++)
	{
		delete g_Enemy[i]; // Deallocates Missile through pointer.
		g_Enemy[i] = nullptr; // Ensures no dangling pointer.
	}
	g_Enemy.clear(); // Removes all elements. Size = 0.
	g_Enemy.shrink_to_fit(); // Sets capacity to size.

	for (unsigned i = 0; i < g_enemyMissiles.size(); i++)
	{
		delete g_enemyMissiles[i]; // Deallocates Missile through pointer.
		g_enemyMissiles[i] = nullptr; // Ensures no dangling pointer.
	}
	g_enemyMissiles.clear(); // Removes all elements. Size = 0.
	g_enemyMissiles.shrink_to_fit(); // Sets capacity to size.

	SDL_DestroyRenderer(g_pRenderer);
	SDL_DestroyWindow(g_pWindow);
	SDL_DestroyTexture(g_pShipTexture);
	SDL_DestroyTexture(g_pPlayerShipTexture);
	SDL_DestroyTexture(g_pEnemyTexture);
	SDL_DestroyTexture(g_pPlayerMissTexture);
	SDL_DestroyTexture(g_pEnemyMissTexture);
	SDL_DestroyTexture(g_pBGTexture);
	//SDL_DestroyTexture(g_pSpaceTexture);

	Mix_FreeChunk(g_pBoom);
	Mix_FreeChunk(g_pMissile);
	Mix_FreeChunk(g_pMissileE);
	Mix_FreeChunk(g_pExplosion);
	Mix_FreeChunk(g_pLaser);
	Mix_FreeMusic(g_pTmnt);
	Mix_FreeMusic(g_pTcats);
	Mix_CloseAudio();
	Mix_Quit();

	IMG_Quit();
	SDL_Quit();
}

void Engine::Wake()
{
	g_start = SDL_GetTicks(); // Gets milliseconds since SDL initialization.
}

void Engine::Sleep()
{
	g_end = SDL_GetTicks();
	g_delta = g_end - g_start; // 1055 - 1050 = 5ms
	if (g_delta < g_fps) // if (5ms < 17ms)
		SDL_Delay(g_fps - g_delta); // Engine sleeps for 12ms.
}




// Run function. Contains the primary game loop.
int Engine::Run()
{
	if (g_running == true) // If engine is already running.
	{
		return 1;
	}
	if (Init("GAME1007_M4_HayeRJ", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, NULL) == false) // If initialization failed.
	{
		return 2;
	}
	// If initialization passes, we are here.
	while (g_running) // Primary game loop. "frame"
	{
		Wake();
		
			HandleEvents(); // Input.
		    Update(); // Processing.
		    Render(); // Output.
		if (g_running == true)
			Sleep();
		if (hits == 10)
			g_running = false;
	}

	

	cout << "GAME OVER" << endl;
	Clean();
	return 0;
}



