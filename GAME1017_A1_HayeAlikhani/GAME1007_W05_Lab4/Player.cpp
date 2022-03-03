#include "Player.h"


Player::Player(int x, int y) :m_src({ 0,0,32,32 }), m_frameCtr(0), m_frameMax(3),
m_spriteIdx(0), m_spriteMin(4), m_spriteMax(5), m_state(IDLE) // Initializers happen before body.
{ // Body considered assignment not initialization.
	m_dst = { x,y,m_src.w * 4,m_src.h * 4 };
	m_spriteIdx = m_spriteMin;
}

void Player::Update()
{
	// Animate.
	if (m_frameCtr++ == m_frameMax)
	{
		m_frameCtr = 0;
		if (++m_spriteIdx == m_spriteMax)
		{
			m_spriteIdx = m_spriteMin;
		}
		m_src.x = 0 + m_src.w * m_spriteIdx;
	}
	// Move.
	// m_dst.y -= MOVESPEED;
}

void Player::SetAnimation(state s, unsigned short min, unsigned short max)
{
	m_state = s;
	m_spriteIdx = m_spriteMin = min;
	m_spriteMax = max;
	m_frameCtr = 0;
}
