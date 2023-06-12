#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
#include "../Scene/MainScene.h"
#include <cassert>

EnemyBase::EnemyBase()
{
}

void EnemyBase::Init()
{
	// 3Dモデルの生成
	pModel_ = std::make_shared<Model>(fileName_);
	pModel_->SetAnimation(animNo_, true, true);
	pModel_->SetUseCollision(true, true);

	// 敵をランダムに配置
	pos_.x = static_cast<float>(GetRand(2000) - 1000);
	pos_.y = 0.0f;
	pos_.z = static_cast<float>(GetRand(2000) - 1000);
	angle_ = static_cast<float>(GetRand(360) * DX_PI_F / 180.0f);
}

void EnemyBase::Draw()
{
	if (!pMainScene_->GetFadeInTheMiddle())
	{
		// ダメージ処理
		if (damageFrame_ > 0)
		{
			if (damageFrame_ % 2) return;
		}

		// モデルの描画
		pModel_->Draw();
	}
}

int EnemyBase::GetModelHandle() const
{
	return pModel_->GetModelHandle();
}

int EnemyBase::GetColFrameIndex() const
{
	return pModel_->GetColFrameIndex();
}