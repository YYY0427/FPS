#include "Enemy.h"
#include "Player.h"
#include "../Model.h"
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
	constexpr int walk_anim = 8;
	constexpr int anim_hit_bullet = 4;
	constexpr int anim_dead = 3;

	// �����蔼�a�̃T�C�Y
	constexpr float col_radius = 70.0f;

	// �ő�HP
	constexpr int max_hp = 30;

	// HP�o�[�̕\��
	constexpr int hp_bar_width = 64;
	constexpr int hp_bar_height = 10;

	// �_���[�W�󂯂����̖��G����
	constexpr int invincible_time = 60;
}

Enemy::Enemy(const char* fileName, std::shared_ptr<Player> pPlayer) :
	pPlayer_(pPlayer),
	m_updateFunc(&Enemy::UpdateToFront),
	animNo_(walk_anim),
	frameCount_(0),
	rotSpeed_(0),
	hp_(max_hp),
	damageFrame_(0),
	isDead_(false),
	fileName_(fileName)
{
	Init();
}

Enemy::~Enemy()
{
}

void Enemy::Init()
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

void Enemy::Update()
{
	(this->*m_updateFunc)();
}

void Enemy::Draw()
{
	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// ���f���̕`��
	pModel_->Draw();
}

void Enemy::DrawUI()
{
	// ���f���̃n���h���擾
	int handle = pModel_->GetModelHandle();

	// ���f�����ɂ���HP�o�[��\��������W�̃f�[�^���擾����
	int frameNo = MV1SearchFrame(handle, "Head3_end");

	// HP�o�[��\��������W�f�[�^�̃��[���h���W���擾����
	VECTOR hpPos = MV1GetFramePosition(handle, frameNo);

	// HP�o�[�\���ʒu�̃��[���h���W���X�N���[�����W�ɕϊ�����
	VECTOR screenPos = ConvWorldPosToScreenPos(hpPos);

	if ((screenPos.z <= 0.0f) || (screenPos.z >= 1.0f))
	{
		// �\���͈͊O�̏ꍇ�����\�����Ȃ�
		return;
	}

	// �ő�HP�ɑ΂��錻�݂�HP�̊������v�Z����
	float hpRate = static_cast<float>(hp_) / static_cast<float>(max_hp);

	// HP�o�[�̒������v�Z����
	float barWidth = hp_bar_width * hpRate;

	// HP�o�[�̓y��(��)
//	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xff0000, true);

	// ���݂�HP(��)
	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x - hp_bar_width / 2 + barWidth, screenPos.y + hp_bar_height, 0x00ff00, true);

	// �g��
	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xffffff, false);
}

int Enemy::GetModelHandle() const
{
	return pModel_->GetModelHandle();
}

int Enemy::GetColFrameIndex() const
{
	return pModel_->GetColFrameIndex();
}

float Enemy::GetColRadius()
{
	return col_radius;
}

void Enemy::OnDamage(int damage)
{
	if (damageFrame_ > 0)	return;
	hp_ -= damage;
	damageFrame_ = invincible_time;

	if (hp_ > 0)
	{
		// �e�������������̃A�j���[�V�����ɐ؂�ւ���
		animNo_ = anim_hit_bullet;
		//	pModel_->ChangeAnimation(anim_hit_bullet, false, false, 60);
		pModel_->SetAnimation(animNo_, false, false);
		m_updateFunc = &Enemy::UpdateHitDamage;
	}
	else
	{
		// ���S�A�j���[�V�����Ɉڍs
		animNo_ = anim_dead;
		pModel_->ChangeAnimation(anim_dead, false, false, 4);
		m_updateFunc = &Enemy::UpdateDead;
	}
}

bool Enemy::IsPlayerFront() const
{
	// ���ݓG�������Ă�������̃x�N�g���𐶐�����
	MATRIX enemyRotMtx = MGetRotY(angle_);
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// �G����v���C���[�ւ̃x�N�g��
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), pos_);
	toPlayer = VNorm(toPlayer);

	// ���ς���p�x�����߂�
	float dot = VDot(dir, toPlayer);
	float angle = acosf(dot);

	return (angle < view_angle);
}

void Enemy::UpdateToPlayer()
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �A�j���[�V�����X�V����
	pModel_->Update();
#if false
	// ���ݓG�������Ă�������̃x�N�g���𐶐�����
	MATRIX enemyRotMtx = MGetRotY(m_angle);
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// �G����v���C���[�ւ̃x�N�g��
	VECTOR toPlayer = VSub(m_pPlayer->GetPos(), m_pos);
	toPlayer = VNorm(toPlayer);

	// ���ς���p�x�����߂�
	float dot = VDot(dir, toPlayer);
	float angle = acosf(dot);

	float rot = -0.05f;

	// ���݂̐i�s�����A�ŏI�I�Ɍ��������������̊O�ς���]���ɂ���
	VECTOR axis = VCross(toPlayer, dir);

	// ����������]���𒆐S�Ƃ�����]�}�g���N�X�𐶐�����
	MATRIX mtx = MGetRotAxis(axis, rot);

	// ���݂̐i�s�����ɉ�]�}�g���N�X��������
	dir = VTransform(dir, mtx);
	m_pos = VAdd(m_pos, VScale(dir, 10.0f));
#else
	// �G����v���C���[�ւ̃x�N�g�������߂�
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), pos_);

	// ���K��
	toPlayer = VNorm(toPlayer);

	// �ړ����x�̔��f
	VECTOR vec = VScale(toPlayer, to_player_speed);

	// �ړ�
	pos_ = VAdd(pos_, vec);
#endif

	// �v���C���[��HP��0��菬�����ꍇ�v���C���[��ǂ�Ȃ�
	if (pPlayer_->GetHP() <= 0)
	{
		m_updateFunc = &Enemy::UpdateToFront;
		frameCount_ = 0;
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));
}

void Enemy::UpdateToFront()
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// ���ݓG�������Ă�������̃x�N�g���𐶐�����
	MATRIX enemyRotMtx = MGetRotY(angle_);

	// MATRIX��VECTOR�ɕϊ�
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// �ړ����x�𔽉f������
	VECTOR vec = VScale(dir, to_front_speed);

	// �ړ�������
	pos_ = VAdd(pos_, vec);

	frameCount_++;
	if (frameCount_ >= 2 * 60)
	{
		if (IsPlayerFront() && pPlayer_->GetHP() > 0)
		{
			m_updateFunc = &Enemy::UpdateToPlayer;
			frameCount_ = 0;
		}
		else
		{
			// ��]����p�x�������_���Ōv�Z
			rotSpeed_ = static_cast<float>(GetRand(250)) * 0.0001f;
			rotSpeed_ += 0.025f;
			if (GetRand(1)) rotSpeed_ *= -1.0f;

			// udpate��ύX
			m_updateFunc = &Enemy::UpdateTurn;
			frameCount_ = 0;
		}	
	}

	pModel_->SetPos(pos_);
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));
}

void Enemy::UpdateTurn()
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �A�j���[�V�����X�V����
	pModel_->Update();

	angle_ += rotSpeed_;

	frameCount_++;
	if (frameCount_ >= 30)
	{
		if (IsPlayerFront() && pPlayer_->GetHP() > 0)
		{
			m_updateFunc = &Enemy::UpdateToPlayer;
			frameCount_ = 0;
		}
		else
		{
			m_updateFunc = &Enemy::UpdateToFront;
			frameCount_ = 0;
		}
	}

	pModel_->SetPos(pos_);
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));
}

void Enemy::UpdateHitDamage()
{
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	assert(animNo_ == anim_hit_bullet);
	pModel_->Update();

	if (pModel_->IsAnimEnd())
	{
		// �ҋ@�A�j���ɕύX����
		animNo_ = walk_anim;
		pModel_->ChangeAnimation(walk_anim, true, true, 4);

		// Update��ҋ@��
		m_updateFunc = &Enemy::UpdateToPlayer;
	}
}

void Enemy::UpdateDead()
{
	frameCount_++;
	assert(animNo_ == anim_dead);
	pModel_->Update();

	if (pModel_->IsAnimEnd() && frameCount_ > 120)
	{
		isDead_ = true;
		frameCount_ = 0;
	}
}
