#include "Missile.h"

Missile::Missile(int x, int y, stateMissile s) :m_src( {0,0,8,22} ),m_frameCtr(0), m_frameMax(3),
	m_spriteIdx(0), m_spriteMax(4) // Initializers happen before body.
{ // Body considered assignment not initialization.
	m_dst = {x,y,m_src.w*2,m_src.h*2};
	state = s;
}

void Missile::Update()
{
	// Animate.
	if (m_frameCtr++ == m_frameMax)
	{
		m_frameCtr = 0;
		if (++m_spriteIdx == m_spriteMax)
		{
			m_spriteIdx = 0;
		}
		m_src.x = 0 + m_src.w * m_spriteIdx;
	}
	// Move.
	if(state == PLAYER)
		m_dst.y -= MOVESPEED;
	if(state == ENEMY)
		m_dst.y += MOVESPEED * 0.5;
}
