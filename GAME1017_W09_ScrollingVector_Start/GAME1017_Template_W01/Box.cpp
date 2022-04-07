#include "Box.h"
#include "Engine.h"
#define SCROLLSPEED 2

Box::Box(const SDL_Point p, bool makeSprite , const SDL_Rect r , const SDL_Color c ):m_pos(p),m_pSprite(nullptr)
{
	if (makeSprite)
	{
		// m_pSprite = new Sprite[m_numSprites];
		m_pSprite = new Sprite(r,c);
	}
}

Box::~Box()
{
	if (m_pSprite != nullptr)
	{
		delete m_pSprite;
	}
}

Box* Box::Clone()
{
	Box* clone = new Box(this->m_pos,false);
	clone->m_pSprite = new Sprite(m_pSprite->m_dst, m_pSprite->m_color);
	return clone;
}

void Box::Update()
{
	m_pos.x -= SCROLLSPEED;
	if (m_pSprite != nullptr)
	{
		
		m_pSprite->m_dst.x -= SCROLLSPEED;
	}
}

void Box::Render()
{
	if (m_pSprite != nullptr)
	{
	
		m_pSprite->Render();
	}
	SDL_Rect dst = { m_pos.x, m_pos.y, 128,128 };
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(),156,230,200,255);
	SDL_RenderDrawRect(Engine::Instance().GetRenderer(), &dst);
}



Sprite::Sprite():m_dst({ 0,0,0,0 }), m_color({255,255,255,255}) {}

Sprite::Sprite(const SDL_Rect r, const SDL_Color c): m_dst(r), m_color(c)  {}

void Sprite::Render()
{
	/*SDL_Rect dst = {m_pos.x, m_pos.y, 128, 128};
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(),m_color.r, m_color.g, m_color.b, m_color.a );
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &m_dst);*/
}
