#include "Enemy.h"


Enemy::Enemy(int x, int y) :m_src({ 0,0,32,32 }), m_frameCtr(0), m_frameMax(3),
m_spriteIdx(0), m_spriteMax(4) // Initializers happen before body.
{ // Body considered assignment not initialization.
	m_dst = { x,y,m_src.w * 4,m_src.h * 4 };
}

void Enemy::Update()
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
	m_dst.y += MOVESPEED;
}
