#include "Shot.h"
#include "Player.h"
#include "../Camera.h"
#include "../Model.h"
#include <cassert>

namespace
{
	// これ以上離れたら消す
	constexpr float erase_distance = 18400.0f;

	// モデルのアドレス
	const char* const bullet_file_name = "Data/Model/bullet.mv1";

	// モデルの拡大率
	constexpr float model_magnification = 25.0f;

	// 当たり半径のサイズ
	constexpr float col_radius = 36.0f;
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
}

void Shot::Init(int handle)
{
	// インスタンス化
	pModel_ = std::make_shared<Model>(handle);

	// モデルの拡大率の設定
	pModel_->SetScale(VGet(model_magnification, model_magnification, model_magnification));
}

void Shot::Update()
{
	if (!isExsit_)	return;

	// 前フレームの位置の取得
	lastPos_ = pos_;

	// まっすぐ進む
	pos_ = VAdd(pos_, vec_);

	// プレイヤーの位置の取得
	VECTOR playerPos = pPlayer_->GetPos();

	// プレイヤーからの距離を求める
	VECTOR toPlayer = VSub(playerPos, pos_);

	// プレイヤーから一定以上離れたら消す
	if (VSize(toPlayer) > erase_distance)
	{
		isExsit_ = false;
	}

	// モデルのポジションの設定
	pModel_->SetPos(pos_);
}

void Shot::Draw()
{
	if (!isExsit_)	return;
	pModel_->Draw();

#ifdef _DEBUG
//	DrawCapsule3D(pos_, lastPos_, col_radius, 8, 0xffffff, 0xff0000, false);
#endif 
}

int Shot::LoadModel() const
{
	// モデルのロード
	// 1回ロード(DuplicateModelを使ってコピーするために必要)
	return MV1LoadModel(bullet_file_name);
}

void Shot::Start(VECTOR pos, VECTOR vec)
{
	// ショット開始
	isExsit_ = true;
	pos_ = pos;
	lastPos_ = pos;
	vec_ = vec;

	// モデルの方向をプレイヤーが向いている方向に設定
	pModel_->SetRot(VGet(-pCamera_->GetCameraAngleY(), pCamera_->GetCameraAngleX(), 0.0f));
}

float Shot::GetColRadius() const
{
	return col_radius;
}
