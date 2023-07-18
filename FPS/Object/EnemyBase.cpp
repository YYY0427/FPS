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
	// �_���[�W����
	if (damageFrame_ > 0)
	{
//		if (damageFrame_ % 2) return;
	}

	// ���f���̕`��
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
	// ���ݓG�������Ă�������̃x�N�g���𐶐�����
	MATRIX enemyRotMtx = MGetRotY(angle_);
	VECTOR dir = VTransform(dir_, enemyRotMtx);

	// �G����^�[�Q�b�g�ւ̃x�N�g��
	VECTOR toTarget = VSub(targetPos, pos_);
	VECTOR toTargetNorm = VNorm(toTarget);

	// ���ς���p�x�����߂�
	float dot = VDot(dir, toTargetNorm);
	float angle = acosf(dot);

	// �G����^�[�Q�b�g�ւ̃x�N�g��
//	float distans = VSize(toTarget);

	// ����̒��Ƀ^�[�Q�b�g������ꍇ
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