#include "SkyDoom.h"
#include "Player.h"
#include "../Model.h"
#include <cassert>

namespace
{
	// アドレス
	const char* const skydoom_file_name = "Data/Model/summerSkyDoom.mv1";
	const char* const cloud1_file_name = "Data/Model/cloud.mv1";
	const char* const cloud2_file_name = "Data/Model/cloud2.mv1";

	// モデルの拡大率
	constexpr float model_magnification = 100.0f;
	constexpr float model_magnification2 = 13.0f;
}

SkyDoom::SkyDoom(): 
	playerPos_(VGet(0, 0, 0)),
	rot_(0.0f)
{
}

SkyDoom::~SkyDoom()
{
}

void SkyDoom::Init()
{
	// インスタンス化
	pSkydoom_ = std::make_shared<Model>(skydoom_file_name);
	pCloud_ = std::make_shared<Model>(cloud1_file_name);
	pCloud2_ = std::make_shared<Model>(cloud2_file_name);

	// プレイヤーの位置の取得
	playerPos_ = pPlayer_->GetPos();

	// モデルのポジションの設定
	pSkydoom_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
	pCloud_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
	pCloud2_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));

	// モデルの拡大率の設定
	pSkydoom_->SetScale(VGet(model_magnification, model_magnification, model_magnification));
	pCloud_->SetScale(VGet(model_magnification2, model_magnification2, model_magnification2));
	pCloud2_->SetScale(VGet(model_magnification2, model_magnification2, model_magnification2));
}

void SkyDoom::Update()
{
	// 回転加算
	rot_ += 0.0001f;

	// プレイヤーの位置の取得
	playerPos_ = pPlayer_->GetPos();

	// モデルのポジションの設定
	pSkydoom_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
	pCloud_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
	pCloud2_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));

	// モデルの回転の設定
	pCloud_->SetRot(VGet(0, rot_, 0));
	pCloud2_->SetRot(VGet(0, rot_, 0));
}

void SkyDoom::Draw()
{
	MV1SetUseZBuffer(pSkydoom_->GetModelHandle(), false);

	// モデルの表示
	pSkydoom_->Draw();
	pCloud_->Draw();
	pCloud2_->Draw();
}
