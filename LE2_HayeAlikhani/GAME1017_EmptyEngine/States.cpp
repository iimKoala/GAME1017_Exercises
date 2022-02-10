#include <iostream>
#include "States.h"
#include "StateManager.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "Engine.h"
#include "tinyxml2.h"
#include <vector>
#include "CollisionManager.h"
#include "Enemy.h"
using namespace std;
using namespace tinyxml2;
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

TitleState::TitleState(){}

void TitleState::Enter()
{
	
}

void TitleState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_N))
	{
		
		STMA::ChangeState( new GameState() );
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

PauseState::PauseState(){}

void PauseState::Enter()
{
	
}

void PauseState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_R))
		STMA::PopState();
}

void PauseState::Render()
{
	// First render the GameState.
	STMA::GetStates().front()->Render();
	// Now render the rest of PauseState.
	SDL_SetRenderDrawBlendMode(Engine::Instance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 255, 255, 128);
	SDL_Rect rect = { 255, 128, 512, 512 };
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	State::Render();
}

void PauseState::Exit()
{
	
}

void GameState::ClearTurrets()
{
	for (unsigned i = 0; i < m_turrets.size(); i++)
	{
		delete m_turrets[i];
		m_turrets[i] = nullptr;
	}
	m_turrets.clear();
	m_turrets.shrink_to_fit();
}

GameState::GameState():m_spawnCtr(0) {}

void GameState::Enter()
{
	TEMA::Load("Img/Turret.png", "turret");
	TEMA::Load("Img/Enemies.png", "enemy");
	s_enemies.push_back(new Enemy({ 512, -200, 40, 57 }));
	// Create the DOM and load the XML file.
	// Iterate through the Turret elements in the file and push_back new Turrets into the m_turrets vector.
		// Look at the last two examples from Week 3

	XMLDocument xmlDoc;
	xmlDoc.LoadFile("TurretPos.xml");
	XMLNode* pRoot = xmlDoc.FirstChildElement("Root");
	// Now let's create objects from the XML file.
	XMLElement* pElement = pRoot->FirstChildElement("Turret");

	

	while (pElement != nullptr)
	{
		int X, Y;

		pElement->QueryIntAttribute("Height", &X);
		pElement->QueryIntAttribute("Width", &Y);
		m_turrets.push_back(new Turret({ X, Y, 100,100 }));
		pElement = pElement->NextSiblingElement("Turret");
	}
}

void GameState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_P))
	{
		STMA::PushState(new PauseState());
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_T))
	{
		m_turrets.push_back(new Turret({ 50,618,100,100 }));
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_C))
	{
		ClearTurrets();
	}
	if (m_spawnCtr++ % 180 == 0)
		s_enemies.push_back(new Enemy({ rand() % (1024 - 40), -57, 40, 57 }));
	for (unsigned i = 0; i < m_turrets.size(); i++)
		m_turrets[i]->Update();
	for (unsigned i = 0; i < s_enemies.size(); i++)
		s_enemies[i]->Update();
	for (unsigned i = 0; i < s_bullets.size(); i++)
		s_bullets[i]->Update();

	// Cleanup bullets and enemies that go off screen.

	for (unsigned i = 0; i < s_bullets.size(); i++)
		if (s_bullets[i]->m_dst.x > WIDTH || s_bullets[i]->m_dst.x < 0 || s_bullets[i]->m_dst.y > HEIGHT || s_bullets[i]->m_dst.y < 0)
		{
			delete s_bullets[i];
			s_bullets[i] = nullptr;
			s_bullets.erase(s_bullets.begin() + i);
			s_bullets.shrink_to_fit();
			cout << "Cleared bullets" << endl;
		}
	for (unsigned i = 0; i < s_enemies.size(); i++)
		if (s_enemies[i]->GetPos().y > HEIGHT)
		{
			delete s_enemies[i];
			s_enemies[i] = nullptr;
			s_enemies.erase(s_enemies.begin() + i);
			s_enemies.shrink_to_fit();
			cout << "Cleared enemy" << endl;
		}
		// for all bullets
			// if bullet goes off screen (four bounds checks)
				// delete s_bullets[i]
				// set s_bullets[i] to nullptr
	
		// for all enemies, similar to above

	// Check for collisions with bullets and enemies.



	
		// for all bullets
			// for all enemies
				// check collision
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());

	for (unsigned i = 0; i < m_turrets.size(); i++)
		m_turrets[i]->Render();
	for (unsigned i = 0; i < s_enemies.size(); i++)
		s_enemies[i]->Render();
	for (unsigned i = 0; i < s_bullets.size(); i++)
		s_bullets[i]->Render();

	SDL_Rect spawnBox = { 50, 618, 100, 100 };
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 255, 255, 255);
	SDL_RenderDrawRect(Engine::Instance().GetRenderer(), &spawnBox);

	// This code below prevents SDL_RenderPresent from running twice in one frame.
	if ( dynamic_cast<GameState*>( STMA::GetStates().back() ) ) // If current state is GameState.
		State::Render();
}

void GameState::Exit()
{
	// You can clear all children of the root node by calling .DeleteChildren(); and this will essentially clear the DOM.
	// Iterate through all the turrets and save their positions as child elements of the root node in the DOM.
	// Make sure to save to the XML file.
	XMLDocument xmlDoc; 
	XMLNode* pRoot = xmlDoc.NewElement("Root");
	xmlDoc.InsertEndChild(pRoot);
	XMLElement* pElement;
	for (unsigned i = 0; i < m_turrets.size(); i++) 
	{
		pElement = xmlDoc.NewElement("Turret");
		pElement->SetAttribute("Width", m_turrets[i]->GetPos().x);
		pElement->SetAttribute("Height", m_turrets[i]->GetPos().y);
		pRoot->InsertEndChild(pElement);
	}
	xmlDoc.SaveFile("TurretPos.xml");

	ClearTurrets();
	for (unsigned i = 0; i < s_enemies.size(); i++)
	{
		delete s_enemies[i];
		s_enemies[i] = nullptr;
	}
	s_enemies.clear();
	s_enemies.shrink_to_fit();
	for (unsigned i = 0; i < s_bullets.size(); i++)
	{
		delete s_bullets[i];
		s_bullets[i] = nullptr;
	}
	s_bullets.clear();
	s_bullets.shrink_to_fit();
	
}

void GameState::Resume()
{

}

// This is how static properties are allocated.
std::vector<Bullet*> GameState::s_bullets;
std::vector<Enemy*> GameState::s_enemies;