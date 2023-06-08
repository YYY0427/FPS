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
	handle_(-1),
	isExsit_(false),
	pos_(VGet(0, 0, 0)),
	lastPos_(VGet(0, 0, 0)),
	vec_(VGet(0, 0, 0))
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
	if (!isExsit_)	return;

	// まっすぐ進む
	lastPos_ = pos_;
	pos_ = VAdd(pos_, vec_);

	// プレイヤーから一定以上離れたら消す
	VECTOR playerPos = pPlayer_->GetPos();
	VECTOR toPlayer = VSub(playerPos, pos_);

	if (VSize(toPlayer) > erase_distance)
	{
		isExsit_ = false;
	}

	MV1SetPosition(handle_, pos_);
}

void Shot::Draw()
{
	if (!isExsit_)	return;
	MV1DrawModel(handle_);
}

int Shot::LoadModel() const
{
	return MV1LoadModel(bullet_file_name);
}

void Shot::Start(VECTOR pos, VECTOR vec)
{
	isExsit_ = true;

	pos_ = pos;
	lastPos_ = pos;
	vec_ = vec;

	// モデルの方向をプレイヤーが向いている方向に設定
	MV1SetRotationXYZ(handle_, VGet(0, pCamera_->GetCameraAngle(), 0));
}