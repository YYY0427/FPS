#include "SkyDoom.h"
#include "Player.h"
#include <cassert>

namespace
{
	// アドレス
//	const char* const file_name = "Data/Model/SkyDoom.mv1";
	const char* const file_name = "Data/Model/SkyDoom.mv1";

	// モデルの拡大率
	constexpr float model_rate = 100.0f;
}

SkyDoom::SkyDoom(): 
	handle_(-1),
	playerPos_(VGet(0, 0, 0))
{
}

SkyDoom::~SkyDoom()
{
	MV1DeleteModel(handle_);
}

void SkyDoom::Init()
{
	handle_ = MV1LoadModel(file_name);
	assert(handle_ != -1);

	playerPos_ = pPlayer_->GetPos();
	MV1SetPosition(handle_, VGet(playerPos_.x, 0, playerPos_.z));
	MV1SetScale(handle_, VGet(model_rate, model_rate, model_rate));
}

void SkyDoom::Update()
{
	playerPos_ = pPlayer_->GetPos();
	MV1SetPosition(handle_, VGet(playerPos_.x, 0, playerPos_.z));
}

void SkyDoom::Draw()
{
	MV1DrawModel(handle_);
}
