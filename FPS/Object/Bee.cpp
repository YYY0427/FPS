#include "Bee.h"
#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
#include "../Collision.h"
#include "Tower.h"
#include <cassert>

namespace
{
	// �G�L�����N�^�[�̌����Ă������
	constexpr VECTOR enemy_dir{ 0.0f, 0.0f, -1.0f };

	// �G�L�����N�^�[�̈ړ����x
	constexpr float to_front_speed = 4.0f;
	constexpr float to_player_speed = 4.0f;

	// �G�L�����N�^�[�̎���p
	constexpr float view_angle = 30.0f * DX_PI_F / 180.0f;

	// �A�j���[�V�����ԍ�
	constexpr int walk_anim_no = 1;
	constexpr int ondamage_anim = 2;
	constexpr int dead_anim_no = 0;
	constexpr int attack_anim_no = 3;

	// �����蔼�a�̃T�C�Y
	constexpr float col_radius = 70.0f;

	// �ő�HP
	constexpr int max_hp = 50;

	// HP�o�[�̕\��
	constexpr int hp_bar_width = 64;
	constexpr int hp_bar_height = 10;

	// �_���[�W�󂯂����̖��G����
	constexpr int invincible_time = 60;

	// �v���C���[�ɍU�����鋗��
	constexpr float attack_distance = 140.0f;
}

Bee::Bee(const char* fileName)
{
	updateFunc_ = &Bee::UpdateToTower;
	animNo_ = walk_anim_no;
	frameCount_ = 0;
	rotSpeed_ = 0;
	hp_ = max_hp;
	damageFrame_ = 0;
	isDead_ = false;
	fileName_ = fileName;
	colRadius_ = col_radius;
	hpBarHeight_ = hp_bar_height;
	hpBarWidth_ = hp_bar_width;
	maxHp_ = max_hp;
	viewAngle_ = view_angle;
	dir_ = enemy_dir;
	deadDisappearTime_ = 120;
	deadAnimNo_ = dead_anim_no;

	Init();
}

Bee::~Bee()
{
}

void Bee::Update()
{
	(this->*updateFunc_)();
}

void Bee::OnDamage(int damage)
{
	// �_���[�W����
	if (damageFrame_ > 0)	return;
	hp_ -= damage;
	damageFrame_ = invincible_time;

	// HP������΃_���[�W�A�j���[�V�����Ɉڍs
	// HP���Ȃ���Ύ��S�A�j���[�V�����Ɉڍs
	if (hp_ > 0)
	{
		// �A�j���[�V�����ݒ�
		animNo_ = ondamage_anim;
		pModel_->ChangeAnimation(animNo_, false, false, 4);

		// update�ύX
		updateFunc_ = &Bee::UpdateHitDamage;
	}
	else
	{
		// ���S�A�j���[�V�����Ɉڍs
		animNo_ = dead_anim_no;
		pModel_->ChangeAnimation(animNo_, false, false, 4);
		updateFunc_ = &Bee::UpdateDead;
	}
}

void Bee::Tracking(VECTOR pos, int target)
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �G����ڕW�ւ̃x�N�g�������߂�
	toTarget_ = VSub(pos, pos_);

	// �p�x�̎擾
	angle_ = static_cast<float>(atan2(toTarget_.x, toTarget_.z));

	// ���K��
	toTarget_ = VNorm(toTarget_);

	// �ړ����x�̔��f
	VECTOR vec = VScale(toTarget_, to_player_speed);

	// �t�B�[���h�Ƃ̓����蔻����s���A���̌��ʂɂ���Ĉړ�
	pos_ = pCollision_->ColisionToField(pModel_->GetModelHandle(), true, false, pos_, vec);

	// �^�[�Q�b�g�܂ł̋���
	float distans = VSize(VSub(pos, pos_));

	// �ڕW�܂ł܂ł̋��������苗���ȓ��Ȃ�U���A�j���[�V�����Ɉڍs
	if (distans < attack_distance)
	{
		// �A�j���[�V�����ݒ�
		animNo_ = attack_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		// ���݌������Ă���Ώۂɂ���čU���Ώۂ�����
		switch (target)
		{
		case player:
			updateFunc_ = &Bee::UpdateAttackToPlayer;
			break;
		case tower:
			updateFunc_ = &Bee::UpdateAttackToTower;
			break;
		}
		frameCount_ = 0;
	}

	// �v���C���[������ł���ꍇ��ǂ�Ȃ�
	if (pPlayer_->GetIsDead())
	{
		updateFunc_ = &Bee::UpdateToFront;
		frameCount_ = 0;
	}

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));
}

void Bee::Attacking(VECTOR pos, int target)
{
	assert(animNo_ == attack_anim_no);

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �v���C���[�܂ł̋���
	float distans = VSize(VSub(pos, pos_));

	// �v���C���[�������̋�������Ă�����v���C���[��ǂ�������
	if (attack_distance < distans)
	{
		// �A�j���[�V�����ݒ�
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		// update��ύX
		switch (target)
		{
		case player:
			updateFunc_ = &Bee::UpdateToPlayer;
			break;
		case tower:
			updateFunc_ = &Bee::UpdateToTower;
		}
		frameCount_ = 0;
	}

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));
}

void Bee::UpdateToPlayer()
{
	Tracking(pPlayer_->GetPos(), player);
}

void Bee::UpdateToTower()
{
	Tracking(pTower_->GetPos(), tower);
}

void Bee::UpdateAttackToPlayer()
{
	Attacking(pPlayer_->GetPos(), player);
}

void Bee::UpdateAttackToTower()
{
	Attacking(pTower_->GetPos(), tower);
}

void Bee::UpdateToFront()
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// ���ݓG�������Ă�������̃x�N�g���𐶐�����
	MATRIX enemyRotMtx = MGetRotY(angle_);

	// MATRIX��VECTOR�ɕϊ�
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// �ړ����x�𔽉f������
	VECTOR vec = VScale(dir, to_front_speed);

	// �t�B�[���h�Ƃ̓����蔻����s���A���̌��ʂɂ���Ĉړ�
	pos_ = pCollision_->ColisionToField(pModel_->GetModelHandle(), true, false, pos_, vec);

	frameCount_++;
	if (frameCount_ >= 2 * 60)
	{
		// �v���C���[����������v���C���[��ǂ�������
		// ������Ȃ��������]����
		if (IsPlayerFront() && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateToPlayer;
			frameCount_ = 0;
		}
		else
		{
			// ��]����p�x�������_���Ōv�Z
			rotSpeed_ = static_cast<float>(GetRand(250)) * 0.0001f;
			rotSpeed_ += 0.025f;
			if (GetRand(1)) rotSpeed_ *= -1.0f;

			// udpate��ύX
			updateFunc_ = &Bee::UpdateTurn;
			frameCount_ = 0;
		}
	}

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));
}

void Bee::UpdateTurn()
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// ��]�̉��Z
	angle_ += rotSpeed_;

	frameCount_++;
	if (frameCount_ >= 30)
	{
		if (IsPlayerFront() && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateToPlayer;
			frameCount_ = 0;
		}
		else
		{
			updateFunc_ = &Bee::UpdateToFront;
			frameCount_ = 0;
		}
	}

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));
}

void Bee::UpdateHitDamage()
{
	assert(animNo_ == ondamage_anim);

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// �A�j���[�V�������I����Ă�����
	if (pModel_->IsAnimEnd())
	{
		// �ҋ@�A�j���ɕύX����
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(walk_anim_no, true, false, 4);

		// Update��ҋ@��
		updateFunc_ = &Bee::UpdateToPlayer;
	}
}