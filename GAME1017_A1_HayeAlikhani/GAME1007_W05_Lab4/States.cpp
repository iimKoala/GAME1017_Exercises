#include <iostream>
#include "States.h"
#include "StateManager.h"

#include "Engine.h"

#include <vector>

#include "Enemy.h"
using namespace std;
Engine E;

void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

TitleState::TitleState() {}

void TitleState::Enter()
{
	cout << "\nPress N to change states" << endl;
}

void TitleState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_N))
	{

		STMA::ChangeState(new GameState() );
	}
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 255, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	State::Render();
}

void TitleState::Exit()
{

}

PauseState::PauseState() {}

void PauseState::Enter()
{

}

void PauseState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_R))
		STMA::PopState();
}

void PauseState::Render()
{
	// First render the GameStatEngine::Instance().
	STMA::GetStates().front()->Render();
	// Now render the rest of PauseStatEngine::Instance().
	SDL_SetRenderDrawBlendMode(Engine::Instance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 255, 255, 128);
	SDL_Rect rect = { 255, 128, 512, 512 };
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	State::Render();
}

void PauseState::Exit()
{

}



GameState::GameState() :m_spawnCtr(0) {}

void GameState::Enter()
{
	
	
	
	
}

void GameState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_P))
	{
		STMA::PushState(new PauseState());
	}
	
	// Determine enemy animation statEngine::Instance().
	switch (Engine::Instance().g_player.m_state)
	{
	case IDLE:
		// Transition to move animation.
		if (Engine::Instance().KeyDown(SDL_SCANCODE_W) || Engine::Instance().KeyDown(SDL_SCANCODE_S) || Engine::Instance().KeyDown(SDL_SCANCODE_A) || Engine::Instance().KeyDown(SDL_SCANCODE_D))
		{
			Engine::Instance().g_player.SetAnimation(MOVE, 0, 5);
		}
		break;
	case MOVE:
		// Transition to idle animation.
		if (!Engine::Instance().KeyDown(SDL_SCANCODE_W) && !Engine::Instance().KeyDown(SDL_SCANCODE_S) && !Engine::Instance().KeyDown(SDL_SCANCODE_A) && !Engine::Instance().KeyDown(SDL_SCANCODE_D))
		{
			Engine::Instance().g_player.SetAnimation(IDLE, 0, 1);
		}
		break;
	}
	Engine::Instance().g_player.Update();
	//cout << "Updating gamEngine::Instance()..." << endl;
	if (Engine::Instance().KeyDown(SDL_SCANCODE_S) && Engine::Instance().g_player.m_dst.y < HEIGHT - 32 * 4) {
		Engine::Instance().g_dst.y += SPEED;
		Engine::Instance().g_player.m_dst.y += SPEED;
	}
	if (Engine::Instance().KeyDown(SDL_SCANCODE_W) && Engine::Instance().g_player.m_dst.y > 0) {
		Engine::Instance().g_dst.y -= SPEED;
		Engine::Instance().g_player.m_dst.y -= SPEED;
	}
	if (Engine::Instance().KeyDown(SDL_SCANCODE_A) && Engine::Instance().g_player.m_dst.x > 0) {
		Engine::Instance().g_dst.x -= SPEED;
		Engine::Instance().g_player.m_dst.x -= SPEED;
	}
	if (Engine::Instance().KeyDown(SDL_SCANCODE_D) && Engine::Instance().g_player.m_dst.x < WIDTH - 32 * 4) {
		Engine::Instance().g_dst.x += SPEED;
	    Engine::Instance().g_player.m_dst.x += SPEED;
	}

	for (unsigned i = 0; i < g_playerMissiles.size(); i++)
	{
		g_playerMissiles[i]->Update();
		// g_missiles.at(i)->Update();
	}

	// Deallocate missiles that go off screen.
	for (unsigned i = 0; i < g_playerMissiles.size(); i++)
	{
		// cout << g_missiles[i]->m_dst.y << " and " << -g_missiles[i]->m_dst.h << endl;
		if (g_playerMissiles[i]->m_dst.y <= -g_playerMissiles[i]->m_dst.h) // -g_missiles[i]->m_dst.h means the entire object off screen
		{
			delete g_playerMissiles[i]; // Deallocates Missile through pointer.
			g_playerMissiles[i] = nullptr; // Ensures no dangling pointer.
			g_playerMissiles.erase(g_playerMissiles.begin() + i); // Erase element and resize array.
			g_playerMissiles.shrink_to_fit();
			// Options:
			break;
			// or i--; instead.
		}
	}


	// Scroll backgrounds.
	Engine::Instance().g_BG1.y += SPEED / 2;
	Engine::Instance().g_BG2.y += SPEED / 2;
	// Check if we need to bounce backgrounds back to original positions.
	if (Engine::Instance().g_BG1.y >= Engine::Instance().g_BG1.h)
	{
		Engine::Instance().g_BG1.y = 0;
		Engine::Instance().g_BG2.y = -768;
	}

	// Timer to spawn enemy ships off screen
	Engine::Instance().timerSpawnEnemy.update();
	if (Engine::Instance().timerSpawnEnemy.getSpawn())
	{
		int rad = rand() % 896;
		//cout << "yeeees" << endl;
		Engine::Instance().timerSpawnEnemy.resetSpawn();
		g_Enemy.push_back(new Enemy(rad, -100));  // Instead of 68 we could add (g_dst.w/2).
		g_Enemy.shrink_to_fit();


	}

	for (unsigned i = 0; i < g_Enemy.size(); i++)
	{
		g_Enemy[i]->Update();
	}

	// Check if enemy ship goes off screen.
	for (unsigned i = 0; i < g_Enemy.size(); i++)
	{
		// cout << g_missiles[i]->m_dst.y << " and " << -g_missiles[i]->m_dst.h << endl;
		if (g_Enemy[i]->m_dst.y >= HEIGHT + 100) // -g_missiles[i]->m_dst.h means the entire object off screen
		{
			delete g_Enemy[i]; // Deallocates Missile through pointer.
			g_Enemy[i] = nullptr; // Ensures no dangling pointer.
			g_Enemy.erase(g_Enemy.begin() + i); // Erase element and resize array.
			g_Enemy.shrink_to_fit();
			// Options:
			break;
			// or i--; instead.
		}
	}

	// Spawn for enemy missiles
	Engine::Instance().timerSpawnEnemyMissile.update();
	if (Engine::Instance().timerSpawnEnemyMissile.getSpawn())
	{
		//cout << "spppppppppown" << endl;
		Engine::Instance().timerSpawnEnemyMissile.resetSpawn();
		for (unsigned i = 0; i < g_Enemy.size(); i++)
		{
			Engine::Instance().g_enemyMissiles.push_back(new Missile(g_Enemy[i]->m_dst.x + g_Enemy[i]->m_dst.w / 2 - 8, g_Enemy[i]->m_dst.y + g_Enemy[i]->m_dst.h - 20, ENEMY));  // Instead of 68 we could add (g_dst.w/2).
			Engine::Instance().g_enemyMissiles.shrink_to_fit();
		}
		Mix_PlayChannel(-1, Engine::Instance().g_pMissileE, 0);
	}

	for (unsigned i = 0; i <Engine::Instance().g_enemyMissiles.size(); i++)
	{
		Engine::Instance().g_enemyMissiles[i]->Update();
	}

	// Deallocate enemy missiles that go off screen. 
	for (unsigned i = 0; i < Engine::Instance().g_enemyMissiles.size(); i++)
	{
		// cout << g_missiles[i]->m_dst.y << " and " << -g_missiles[i]->m_dst.h << endl;
		if (Engine::Instance().g_enemyMissiles[i]->m_dst.y >= HEIGHT + 100) // -g_missiles[i]->m_dst.h means the entire object off screen
		{
			delete Engine::Instance().g_enemyMissiles[i]; // Deallocates Missile through pointer.
			Engine::Instance().g_enemyMissiles[i] = nullptr; // Ensures no dangling pointer.
			Engine::Instance().g_enemyMissiles.erase(Engine::Instance().g_enemyMissiles.begin() + i); // Erase element and resize array.
			Engine::Instance().g_enemyMissiles.shrink_to_fit();
			// Options:
			break;
			// or i--; instead.
		}
	}

	// Check if the player missile hits enemy
	for (unsigned i = 0; i < g_playerMissiles.size(); i++) // For each missilEngine::Instance().
	{

		for (unsigned j = 0; j < g_Enemy.size(); j++)
		{
			if (SDL_HasIntersection(&g_playerMissiles[i]->m_dst, &g_Enemy[j]->m_dst)) // Collision check. AABB.
			{
				Mix_PlayChannel(-1, Engine::Instance().g_pExplosion, 0);
				// Deallocate missile and enemy.
				delete g_playerMissiles[i]; // Deallocates Missile through pointer.
				g_playerMissiles[i] = nullptr; // Ensures no dangling pointer.
				g_playerMissiles.erase(g_playerMissiles.begin() + i); // Erase element and resize array.
				g_playerMissiles.shrink_to_fit();
				delete g_Enemy[j]; // Deallocates Enemy through pointer.
				g_Enemy[j] = nullptr; // Ensures no dangling pointer.
				g_Enemy.erase(g_Enemy.begin() + j); // Erase element and resize array.
				g_Enemy.shrink_to_fit();
				Engine::Instance().kills++; // counter for number of enemy killed.
				cout << "Number of kills: " << Engine::Instance().kills << endl;
				break;
			}
		}
	}
	if (Engine::Instance().Alive)
	{
		// Check if enemy missiles hit the player
		for (unsigned i = 0; i < Engine::Instance().g_enemyMissiles.size(); i++) // For each missilEngine::Instance().
		{


			if (SDL_HasIntersection(&Engine::Instance().g_enemyMissiles[i]->m_dst, &Engine::Instance().g_player.m_dst)) // Collision check. AABB.
			{
				Engine::Instance().Alive = false;
				cout << "YOU GOT HIT!" << endl;
				Mix_PlayChannel(-1, Engine::Instance().g_pExplosion, 0);
				// Deallocate missilEngine::Instance().
				delete Engine::Instance().g_enemyMissiles[i]; // Deallocates Missile through pointer.
				Engine::Instance().g_enemyMissiles[i] = nullptr; // Ensures no dangling pointer.
				Engine::Instance().g_enemyMissiles.erase(Engine::Instance().g_enemyMissiles.begin() + i); // Erase element and resize array.
				Engine::Instance().g_enemyMissiles.shrink_to_fit();
				Engine::Instance().hits++; // Counter for number of hits that hits the player
				cout << "Number of hits: " << Engine::Instance().hits << endl;
				SDL_DestroyTexture(Engine::Instance().g_pPlayerShipTexture);
				Engine::Instance().g_player = NULL;

				break;
			}

		}

		// Check if the enemy hits the player
		for (unsigned i = 0; i < g_Enemy.size(); i++) // For each missilEngine::Instance().
		{


			if (SDL_HasIntersection(&g_Enemy[i]->m_dst, &Engine::Instance().g_player.m_dst)) // Collision check. AABB.
			{
				Engine::Instance().Alive = false;
				cout << "YOU GOT HIT!" << endl;
				Mix_PlayChannel(-1, Engine::Instance().g_pExplosion, 0);
				Engine::Instance().hits++; // Counter for number of hits that hits the player
				cout << "Number of hits: " << Engine::Instance().hits << endl;
				SDL_DestroyTexture(Engine::Instance().g_pPlayerShipTexture);
				Engine::Instance().g_player = NULL;

				break;
			}

		}
	}

}

void GameState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	//cout << "Rendering changes to window..." << endl;
	SDL_RenderCopy(Engine::Instance().GetRenderer(), Engine::Instance().g_pBGTexture, NULL, &Engine::Instance().g_BG1); // For the bg1.
	SDL_RenderCopy(Engine::Instance().GetRenderer(), Engine::Instance().g_pBGTexture, NULL, &Engine::Instance().g_BG2); // For the bg2.
	//SDL_RenderCopy(g_pRenderer, g_pSpaceTexture, NULL, NULL); // For the bg.
	if (Engine::Instance().g_fire == true)
	{
		SDL_Point origin = { Engine::Instance().g_dst.x + Engine::Instance().g_dst.w / 2, Engine::Instance().g_dst.y + Engine::Instance().g_dst.h / 2 };
		SDL_Point end = { Engine::Instance().g_dst.x + Engine::Instance().g_dst.w / 2, Engine::Instance().g_dst.y - 300 };
		SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 255, 0, 255);
		SDL_RenderDrawLine(Engine::Instance().GetRenderer(), origin.x, origin.y, end.x, end.y);
	}


	// Render player missiles.
	for (unsigned i = 0; i < g_playerMissiles.size(); i++)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), Engine::Instance().g_pPlayerMissTexture,
			&(g_playerMissiles[i]->m_src), &(g_playerMissiles[i]->m_dst));
	}

	// Render enemy missiles.
	for (unsigned i = 0; i < Engine::Instance().g_enemyMissiles.size(); i++)
	{
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), Engine::Instance().g_pEnemyMissTexture,
			&(Engine::Instance().g_enemyMissiles[i]->m_src), &(Engine::Instance().g_enemyMissiles[i]->m_dst), 0.0, NULL, SDL_FLIP_VERTICAL);
	}

	// Render enemy ships
	for (unsigned i = 0; i < g_Enemy.size(); i++)
	{
		/*SDL_RenderCopy(g_pRenderer, g_pEnemy2Texture,
			&(g_Enemy[i]->m_src), &(g_Enemy[i]->m_dst));*/
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), Engine::Instance().g_pEnemy2Texture, &(g_Enemy[i]->m_src), &(g_Enemy[i]->m_dst), 0.0, NULL, SDL_FLIP_VERTICAL); // For the Sonic.
	}

	SDL_RenderCopy(Engine::Instance().GetRenderer(), Engine::Instance().g_pPlayerShipTexture, &Engine::Instance().g_player.m_src, &Engine::Instance().g_player.m_dst); // For the ship.
	if (Engine::Instance().g_turret == true)
	{
		SDL_Point origin = { Engine::Instance().g_dst.x + Engine::Instance().g_dst.w / 2, Engine::Instance().g_dst.y + Engine::Instance().g_dst.h / 2 };
		SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 255, 255);
		SDL_RenderDrawLine(Engine::Instance().GetRenderer(), origin.x, origin.y, Engine::Instance().g_mousePos.x, Engine::Instance().g_mousePos.y);
	}
	SDL_RenderPresent(Engine::Instance().GetRenderer()); // Flip buffers - send data to window.
	if (dynamic_cast<GameState*>(STMA::GetStates().back())) // If current state is GameStatEngine::Instance().
		State::Render();
}

void GameState::Exit()
{
	// You can clear all children of the root node by calling .DeleteChildren(); and this will essentially clear the DOM.
	// Iterate through all the turrets and save their positions as child elements of the root node in the DOM.
	// Make sure to save to the XML filEngine::Instance().
	
	for (unsigned i = 0; i < g_Enemy.size(); i++)
	{
		delete g_Enemy[i];
		g_Enemy[i] = nullptr;
	}
	g_Enemy.clear();
	g_Enemy.shrink_to_fit();
	for (unsigned i = 0; i < g_playerMissiles.size(); i++)
	{
		delete g_playerMissiles[i];
		g_playerMissiles[i] = nullptr;
	}
	g_playerMissiles.clear();
	g_playerMissiles.shrink_to_fit();

}

void GameState::Resume()
{

}

// This is how static properties are allocated.
std::vector<Missile*> GameState::g_playerMissiles;
std::vector<Enemy*> GameState::g_Enemy;