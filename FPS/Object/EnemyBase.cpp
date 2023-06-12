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
	// 3D���f���̐���
	pModel_ = std::make_shared<Model>(fileName_);
	pModel_->SetAnimation(animNo_, true, true);
	pModel_->SetUseCollision(true, true);

	// �G�������_���ɔz�u
	pos_.x = static_cast<float>(GetRand(2000) - 1000);
	pos_.y = 0.0f;
	pos_.z = static_cast<float>(GetRand(2000) - 1000);
	angle_ = static_cast<float>(GetRand(360) * DX_PI_F / 180.0f);
}

void EnemyBase::Draw()
{
	if (!pMainScene_->GetFadeInTheMiddle())
	{
		// �_���[�W����
		if (damageFrame_ > 0)
		{
			if (damageFrame_ % 2) return;
		}

		// ���f���̕`��
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