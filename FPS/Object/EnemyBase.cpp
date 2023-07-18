#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
#include "../UI.h"
#include <cassert>

EnemyBase::EnemyBase()
{
	pUI_ = std::make_shared<UI>();
}

EnemyBase::~EnemyBase()
{

}

void EnemyBase::Draw()
{
	// ダメージ処理
	if (damageFrame_ > 0)
	{
//		if (damageFrame_ % 2) return;
	}

	// モデルの描画
	pModel_->Draw();
}

void EnemyBase::DrawUI()
{
	pUI_->DrawHpUI(pModel_->GetModelHandle(), hp_, maxHp_, "Head3_end", 30.0f);
}

int EnemyBase::GetModelHandle() const
{
	return pModel_->GetModelHandle();
}

int EnemyBase::GetColFrameIndex() const
{
	return pModel_->GetColFrameIndex();
}

bool EnemyBase::IsPlayerFront(VECTOR targetPos) const
{
	// 現在敵が向いている方向のベクトルを生成する
	MATRIX enemyRotMtx = MGetRotY(angle_);
	VECTOR dir = VTransform(dir_, enemyRotMtx);

	// 敵からターゲットへのベクトル
	VECTOR toTarget = VSub(targetPos, pos_);
	VECTOR toTargetNorm = VNorm(toTarget);

	// 内積から角度を求める
	float dot = VDot(dir, toTargetNorm);
	float angle = acosf(dot);

	// 敵からターゲットへのベクトル
//	float distans = VSize(toTarget);

	// 視野の中にターゲットがいる場合
	if (angle < viewAngle_)
	{
		return true;
	}
	return false;
}

void EnemyBase::UpdateDead()
{
	frameCount_++;
	assert(animNo_ == deadAnimNo_);
	pModel_->Update();

	if (pModel_->IsAnimEnd() && frameCount_ > deadDisappearTime_)
	{
		isDead_ = true;
		frameCount_ = 0;
	}
}