#include "States.h"
#include "StateManager.h"
#include "TextureManager.h"
#include "Engine.h"
#include "EventManager.h"
#include "CollisionManager.h"
#include "SoundManager.h"
#include "Primitives.h"
#include "Button3.h"
#include "AsteroidSprites.h"

#include <iostream>
using namespace std;

void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

void State::Resume(){}

GameObject* State::GetGo(const std::string& s)
{ // Using 'std' just to show origin.
	auto it = std::find_if(m_objects.begin(), m_objects.end(), 
		// Lambda expression/function. An in-function function.
		[&](const std::pair<std::string, GameObject*>& element)
		{
			return element.first == s;
		} 
	); // End of find_if.
	if (it != m_objects.end())
		return it->second;
	else return nullptr;
}

auto State::GetIt(const std::string& s)
{ 
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[&](const std::pair<std::string, GameObject*>& element)
		{
			return element.first == s;
		});
	return it;
}

// Begin TitleState
TitleState::TitleState(){}

void TitleState::Enter()
{
	TEMA::Load("Img/Title.png", "title");
	TEMA::Load("Img/button.png", "play");
	TEMA::Load("Img/TitleBack.jpg", "bg");
	SOMA::Load("Aud/Title.mp3", "title", SOUND_MUSIC);
	m_objects.push_back(pair<string, GameObject*>("bg",
		new Image({ 0, 0, 1920, 1200 }, { 0, 0, 1024, 768 }, "bg")));
	m_objects.push_back(pair<string, GameObject*>("title",
		new Image({ 0, 0, 800, 156 }, { 112, 100, 800, 156 }, "title")));
	m_objects.push_back(pair<string, GameObject*>("play",
		new PlayButton({ 0, 0, 400, 100 }, { 412, 384, 200, 50 }, "play")));
	SOMA::AllocateChannels(16);
	SOMA::SetMusicVolume(32);
	SOMA::PlayMusic("title", -1, 2000);
}

void TitleState::Update()
{
	for (auto const& i : m_objects)
	{
		i.second->Update();
		if (STMA::StateChanging()) return;
	}
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	for (auto const& i : m_objects)
		i.second->Render();
	State::Render();
}

void TitleState::Exit()
{
	TEMA::Unload("title");
	TEMA::Unload("play");
	TEMA::Unload("bg");
	SOMA::StopMusic();
	SOMA::Unload("title", SOUND_MUSIC);
	for (auto& i : m_objects)
	{
		delete i.second;
		i.second = nullptr; // ;)
	}
}
// End TitleState

// Begin GameState
GameState::GameState(){}

void GameState::Enter() // Used for initialization.
{
	TEMA::Load("Img/background.png", "bg");
	TEMA::Load("Img/Sprites.png", "sprites");
	SOMA::Load("Aud/Engines.wav", "engines", SOUND_SFX);
	SOMA::Load("Aud/Fire.wav", "fire", SOUND_SFX);
	SOMA::Load("Aud/Explode.wav", "explode", SOUND_SFX);
	SOMA::Load("Aud/Wings.mp3", "wings", SOUND_MUSIC);
	//SOMA::Load("Aud/Danger.mp3", "danger", SOUND_MUSIC); // Alternate music track.
	m_objects.push_back(pair<string, GameObject*>("bg",
		new Image({ 0, 0, 1024, 768 }, { 0, 0, 1024, 768 }, "bg")));
	m_objects.push_back(pair<string, GameObject*>("astf",
		new AsteroidField(5)));
	m_objects.push_back(pair<string, GameObject*>("ship",
		new ShipAsteroids({ 0, 0, 100, 100 }, { 462.0f, 334.0f, 100.0f, 100.0f })));
	SOMA::SetSoundVolume(16);
	SOMA::SetMusicVolume(32);
	SOMA::PlayMusic("wings", -1, 2000);
	//SOMA::PlayMusic("danger", -1, 2000); // Alternate music track.
}

void GameState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_X))
	{
		STMA::ChangeState(new TitleState());
		return;
	}
	for (auto const& i : m_objects)
	{
		i.second->Update();
		if (STMA::StateChanging()) return; // Not needed currently, because no buttons that trigger state change.
	}
	// Check collision. 
	if (GetGo("ship") != nullptr)
	{
		// Creating some temporary fields (all as pointers for consistency) for convenience.
		vector<Asteroid*>* field = &static_cast<AsteroidField*>(GetGo("astf"))->GetAsteroids();
		ShipAsteroids* ship = static_cast<ShipAsteroids*>(GetGo("ship"));
		// Player vs. asteroids first.
		for (unsigned int i = 0; i < field->size(); i++)
		{
			Asteroid* ast = field->at(i);
			if (COMA::CircleCircleCheck(ship->GetCenter(), ast->GetCenter(),
				ship->GetRadius(), ast->GetRadius()))
			{
				SOMA::PlaySound("explode");
				delete ship;
				m_objects.erase(GetIt("ship")); // Erases whole ship std::pair.
				m_objects.shrink_to_fit();
				return;
			}
		}
		// Bullets vs. asteroids. With temp fields for bullets.
		vector<Bullet*>* bullets = &ship->GetBullets();
		for (unsigned int i = 0; i < bullets->size(); i++)
		{
			Bullet* bul = bullets->at(i);
			for (unsigned int j = 0; j < field->size(); j++)
			{
				Asteroid* ast = field->at(j);
				if (COMA::CircleCircleCheck(bul->GetCenter(), ast->GetCenter(),
					bul->GetRadius(), ast->GetRadius()))
				{
					SOMA::PlaySound("explode");
					// New asteroid chunk spawn code. Hints:
					for (unsigned int a = 0; a < field->size(); a++)
					{
						
						field->erase(field->begin() + a);
						field->shrink_to_fit();
					}
					for (unsigned int a = 0; a > field->size(); a++)
					{
						
						field->erase(field->begin() + a);
						field->shrink_to_fit();


					}
					// You would only need to spawn two chunks if the asteroid that is hit is full size or one smaller than full.
					// As yourself why the bullet and asteroid that are colliding are only getting destroyed AFTER the two chunks spawn.
					// What data can you get from the bullet and asteroid that the chunks need?

					// End new chunk spawn code.
					delete bul;
					bullets->erase(bullets->begin() + i);
					bullets->shrink_to_fit();
					delete ast;
					field->erase(field->begin() + j);
					field->shrink_to_fit();
					return;
				}
			}
		}
	}
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	for (auto const& i : m_objects)
		i.second->Render();
	if ( dynamic_cast<GameState*>(STMA::GetStates().back()) ) // Check to see if current state is of type GameState
		State::Render();
}

void GameState::Exit()
{
	TEMA::Unload("bg");
	TEMA::Unload("sprites");
	SOMA::StopSound();
	SOMA::StopMusic();
	SOMA::Unload("engines", SOUND_SFX);
	SOMA::Unload("fire", SOUND_SFX);
	SOMA::Unload("explode", SOUND_SFX);
	SOMA::Unload("wings", SOUND_MUSIC);
	for (auto& i : m_objects)
	{
		delete i.second; // De-allocating GameObject*s
		i.second = nullptr; // ;)
	}
}

void GameState::Resume(){}
// End GameState