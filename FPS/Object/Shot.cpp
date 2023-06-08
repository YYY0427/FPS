#include "Shot.h"
#include "Player.h"
#include "../Camera.h"
#include <cassert>

namespace
{
	// これ以上離れたら消す
	constexpr float erase_distance = 3000.0f;

	// 弾のアドレス
	const char* const bullet_file_name = "Data/Model/bullet.mv1";
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
	MV1DeleteModel(handle_);
}

void Shot::Init(int handle)
{
	handle_ = handle;
	assert(handle_ != -1);

	MV1SetScale(handle_, VGet(25, 25, 25));
	MV1SetRotationXYZ(handle_, VGet(0, pCamera_->GetCameraAngle(), 0));
}

void Shot::Update()
{
	if (!m_isExsit)	return;

	// まっすぐ進む
	m_lastPos = m_pos;
	m_pos = VAdd(m_pos, m_vec);

	// プレイヤーから一定以上離れたら消す
	VECTOR playerPos = pPlayer_->GetPos();
	VECTOR toPlayer = VSub(playerPos, m_pos);

	if (VSize(toPlayer) > erase_distance)
	{
		m_isExsit = false;
	}

	MV1SetPosition(handle_, m_pos);
}

void Shot::Draw()
{
	if (!m_isExsit)	return;
	MV1DrawModel(handle_);
}

int Shot::LoadModel() const
{
	return MV1LoadModel(bullet_file_name);
}

void Shot::Start(VECTOR pos, VECTOR vec)
{
	m_isExsit = true;

	m_pos = pos;
	m_lastPos = pos;
	m_vec = vec;

	// モデルの方向をプレイヤーが向いている方向に設定
	MV1SetRotationXYZ(handle_, VGet(0, pCamera_->GetCameraAngle(), 0));
}