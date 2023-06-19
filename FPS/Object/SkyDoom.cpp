#include "SkyDoom.h"
#include "Player.h"
#include "../Model.h"
#include <cassert>

namespace
{
	// アドレス
	const char* const file_name = "Data/Model/skyDoom.mv1";

	// モデルの拡大率
	constexpr float model_magnification = 100.0f;
}

SkyDoom::SkyDoom(): 
	playerPos_(VGet(0, 0, 0))
{
}

SkyDoom::~SkyDoom()
{
}

void SkyDoom::Init()
{
	// インスタンス化
	pModel_ = std::make_shared<Model>(file_name);

	// プレイヤーの位置の取得
	playerPos_ = pPlayer_->GetPos();

	// モデルのポジションの設定
	pModel_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));

	// モデルの拡大率の設定
	pModel_->SetScale(VGet(model_magnification, model_magnification, model_magnification));
}

void SkyDoom::Update()
{
	// プレイヤーの位置の取得
	playerPos_ = pPlayer_->GetPos();

	// モデルのポジションの設定
	pModel_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
}

void SkyDoom::Draw()
{
	// モデルの表示
	pModel_->Draw();
}
