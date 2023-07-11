#include "EnemyShot.h"
#include "Model.h"
#include "Object/EnemyManager.h"
#include "Object/Tower.h"

namespace
{
	// ファイルパス
	const char* const shot_adress = "Data/Model/sord.mv1";

	// モデルの拡大率
	constexpr float model_magnification = 25.0f;

	// 当たり半径のサイズ
	constexpr float col_radius = 36.0f;

	// これ以上離れたら消す
	constexpr float erase_distance = 18400.0f;
}

EnemyShot::EnemyShot()
{
}

EnemyShot::~EnemyShot()
{
}

void EnemyShot::Init(int handle)
{
	// インスタンス化
	pModel_ = std::make_shared<Model>(handle);

	// モデルの拡大率の設定
	pModel_->SetScale(VGet(model_magnification, model_magnification, model_magnification));
}

void EnemyShot::Update()
{
	if (!isExsit_)	return;

	// 前フレームの位置の取得
	lastPos_ = pos_;

	// まっすぐ進む
	pos_ = VAdd(pos_, vec_);

	// プレイヤーの位置の取得
	VECTOR towerPos = pTower_->GetPos();

	// プレイヤーからの距離を求める
	VECTOR toPlayer = VSub(towerPos, pos_);

	// プレイヤーから一定以上離れたら消す
	if (VSize(toPlayer) > erase_distance)
	{
		isExsit_ = false;
	}

	// モデルのポジションの設定
	pModel_->SetPos(pos_);

	// FIXME:
	// カメラの回転
//	pModel_->SetRot(VGet(pCamera_->GetCameraPitch() + DX_PI_F / 5, pCamera_->GetCameraYaw() + DX_PI_F, 0.0f));
}

void EnemyShot::Draw()
{
	if (!isExsit_)	return;
	pModel_->Draw();
}

int EnemyShot::LoadModel() const
{
	// モデルのロード
	// 1回ロード(DuplicateModelを使ってコピーするために必要)
	return MV1LoadModel(shot_adress);
}

void EnemyShot::Start(VECTOR pos, VECTOR vec)
{
	// ショット開始
	isExsit_ = true;
	pos_ = pos;
	lastPos_ = pos;
	vec_ = vec;

	// モデルの方向をプレイヤーが向いている方向に設定
//	pModel_->SetRot(VGet(pCamera_->GetCameraPitch(), pCamera_->GetCameraYaw(), 0.0f));

	// FIXME:
	// カメラの回転
//	pModel_->SetRot(VGet(pCamera_->GetCameraPitch() + DX_PI_F / 5, pCamera_->GetCameraYaw() + DX_PI_F, 0.0f));
}

float EnemyShot::GetColRadius() const
{
	return col_radius;
}
