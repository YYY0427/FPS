#include "EnemyShot.h"
#include "Model.h"
#include "Object/Player.h"
#include "Object/Tower.h"
#include "Object/EnemyBase.h"

namespace
{
	// これ以上離れたら消す
	constexpr float erase_distance = 18400.0f;

	// モデルの拡大率
	constexpr float model_scale = 0.1f;

	// モデルの当たり判定の半径
	constexpr float col_radius = 16.0f;

	constexpr VECTOR init_direction{ 0, 1, 0 };
}

EnemyShot::EnemyShot(int handle, const VECTOR& pos, const VECTOR& vec, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, int target) :
	pos_(pos),
	vec_(vec),
	initPos_(pos),
	lastPos_(VGet(0, 0, 0)),
	isEnabled_(true), 
	handle_(handle),
	pPlayer_(pPlayer),
	pTower_(pTower),
	target_(target)
{
	switch (target_)
	{
	case EnemyBase::Target::player:
		targetPos_ = pPlayer_->GetPos();
		break;
	case EnemyBase::Target::tower:
		targetPos_ = pTower_->GetPos();
		break;
	}

	// モデルの作成(インスタンス化)
	pModel_ = std::make_shared<Model>(handle_);

	pModel_->SetUseCollision(true, true);

	// ターゲットまでのベクトルの作成
	VECTOR toTargetVec = VSub(targetPos_, pos_);

	// 拡大行列の作成
	scaleMatrix_ = MGetScale(VGet(model_scale, model_scale, model_scale));

	// ターゲットの方向の回転行列の作成
	rotateMatrix_ = MGetRotVec2(init_direction, toTargetVec);
}

EnemyShot::~EnemyShot()
{
}

void EnemyShot::Update()
{
	// ポジションの保存
	lastPos_ = pos_;

	// 移動
	pos_ = VAdd(pos_, vec_);

	// 平行行列の作成
	MATRIX transMatrix = MGetTranslate(pos_);

	// 初期位置から現在の位置までのベクトルの作成
	VECTOR initPosToNowPosVec= VSub(initPos_, pos_);

	// ターゲットからある程度の距離離れたら消す
	if (VSize(initPosToNowPosVec) > erase_distance)
	{
		isEnabled_ = false;
	}

	// 行列の合成
	MATRIX mat = MMult(scaleMatrix_, transMatrix);
	MATRIX mat2 = MMult(rotateMatrix_, mat);

	// モデルの座標変換用行列をセットする
	MV1SetMatrix(pModel_->GetModelHandle(), mat2);

	pModel_->Update();
}

void EnemyShot::Draw()
{
	DrawCapsule3D(pos_, lastPos_, col_radius, 16, 0xff0000, 0xff0000, false);
	pModel_->Draw();
}

float EnemyShot::GetColRadius() const
{
	return col_radius;
}
