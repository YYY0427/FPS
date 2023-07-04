#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
#include <cassert>

EnemyBase::EnemyBase()
{
}

EnemyBase::~EnemyBase()
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

	pModel_->Update();
}

void EnemyBase::Draw()
{
	// �_���[�W����
	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// ���f���̕`��
	pModel_->Draw();
}

void EnemyBase::DrawUI()
{
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	VECTOR startPos = ConvScreenPosToWorldPos(VGet(mouseX, mouseY, 0.0f));
	VECTOR endPos = ConvScreenPosToWorldPos(VGet(mouseX, mouseY, 1.0f));

	// ���f���̃n���h���擾
	int handle = pModel_->GetModelHandle();

	MV1SetupCollInfo(handle, -1, 8, 8, 8);

	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(handle, -1, startPos, endPos);
	if (hitPoly.HitFlag)
	{
		// ���f�����ɂ���HP�o�[��\��������W�̃f�[�^���擾����
		int frameNo = MV1SearchFrame(handle, "Head3_end");

		// HP�o�[��\��������W�f�[�^�̃��[���h���W���擾����
		VECTOR hpPos = MV1GetFramePosition(handle, frameNo);

		hpPos.y += 30;

		// HP�o�[�\���ʒu�̃��[���h���W���X�N���[�����W�ɕϊ�����
		VECTOR screenPos = ConvWorldPosToScreenPos(hpPos);

		if ((screenPos.z <= 0.0f) || (screenPos.z >= 1.0f))
		{
			// �\���͈͊O�̏ꍇ�����\�����Ȃ�
			return;
		}

		// �ő�HP�ɑ΂��錻�݂�HP�̊������v�Z����
		float hpRate = static_cast<float>(hp_) / static_cast<float>(maxHp_);

		// HP�o�[�̒������v�Z����
		float barWidth = hpBarWidth_ * hpRate;

		// HP�o�[�̓y��(��)
	//	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xff0000, true);

		// ���݂�HP(��)
		DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x - hpBarWidth_ / 2 + barWidth, screenPos.y + hpBarHeight_, 0x00ff00, true);

		// �g��
		DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x + hpBarWidth_ / 2, screenPos.y + hpBarHeight_, 0xffffff, false);

#ifdef _DEBUG
		//	DrawSphere3D(pos_, col_radius, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), false);
#endif
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

bool EnemyBase::IsPlayerFront() const
{
	// ���ݓG�������Ă�������̃x�N�g���𐶐�����
	MATRIX enemyRotMtx = MGetRotY(angle_);
	VECTOR dir = VTransform(dir_, enemyRotMtx);

	// �G����v���C���[�ւ̃x�N�g��
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), pos_);
	VECTOR toPlayerNorm = VNorm(toPlayer);

	// ���ς���p�x�����߂�
	float dot = VDot(dir, toPlayerNorm);
	float angle = acosf(dot);

	// �G����v���C���[�ւ̃x�N�g��
	float distans = VSize(toPlayer);

	// ����̒��Ƀv���C���[�����ăv���C���[�܂ł̋������w��̋����ȓ��̏ꍇ
	// �v���C���[��ǂ�������
	if (angle < viewAngle_ && distans < 1000.0f)
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