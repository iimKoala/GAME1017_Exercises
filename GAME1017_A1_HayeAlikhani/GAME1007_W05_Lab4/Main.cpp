#include <iostream>
#include "Engine.h"
#include "StateManager.h"
#include "States.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Missile.h"
#include "Player.h"
#include "Enemy.h"
#include "Timer.h"
#include <vector> 



int main(int argc, char* argv[])
{
	return Engine::Instance().Run();
}

