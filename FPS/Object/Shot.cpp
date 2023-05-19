#include "Shot.h"
#include "Player.h"

namespace
{
	// ショットの半径
	constexpr float radius = 48.0f;

	// これ以上離れたら消す
	constexpr float erase_distance = 3000.0f;
}

Shot::Shot() :
	m_isExsit(false),
	m_pos(VGet(0, 0, 0)),
	m_lastPos(VGet(0, 0, 0)),
	m_vec(VGet(0, 0, 0))
{
}

Shot::~Shot()
{
}

void Shot::Init()
{
}

void Shot::Update()
{
	if (!m_isExsit)	return;

	// まっすぐ進む
	m_lastPos = m_pos;
	m_pos = VAdd(m_pos, m_vec);

	// プレイヤーから一定以上離れたら消す
//	VECTOR playerPos = pPlayer_->GetPos();
//	VECTOR toPlayer = VSub(playerPos, m_pos);

	//if (VSize(toPlayer) > erase_distance)
	//{
	//	m_isExsit = false;
	//}
}

void Shot::Draw()
{
	if (!m_isExsit)	return;

	//	DrawSphere3D(m_pos, radius, 8, 0xffffff, 0xffffff, true);
	DrawCapsule3D(m_pos, m_lastPos, radius, 8, 0xffff00, 0xffff00, true);
}

void Shot::Start(VECTOR pos, VECTOR vec)
{
	m_isExsit = true;

	m_pos = pos;
	m_lastPos = pos;
	m_vec = vec;
}

float Shot::GetRadius() const
{
	return radius;
}