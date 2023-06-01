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
	m_animNo(walk_anim),
	m_frameCount(0),
	m_rotSpeed(0),
	m_hp(max_hp),
	m_damageFrame(0),
	isDead_(false)
{
	// 3D���f���̐���
	pModel_ = std::make_shared<Model>(fileName);
	pModel_->SetAnimation(m_animNo, true, true);
	pModel_->SetUseCollision(true, true);

	// �G�������_���ɔz�u
	m_pos.x = GetRand(2000) - 1000;
	m_pos.y = 0;
	m_pos.z = GetRand(2000) - 1000;
	m_angle = GetRand(360) * DX_PI_F / 180.0f;
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
}

void Enemy::Update()
{
	(this->*m_updateFunc)();
}

void Enemy::Draw()
{
	if (m_damageFrame > 0)
	{
		if (m_damageFrame % 2) return;
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
	float hpRate = static_cast<float>(m_hp) / static_cast<float>(max_hp);

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
	if (m_damageFrame > 0)	return;

	m_hp -= damage;
	m_damageFrame = invincible_time;

	if (m_hp > 0)
	{
		// �e�������������̃A�j���[�V�����ɐ؂�ւ���
		m_animNo = anim_hit_bullet;
		//	pModel_->ChangeAnimation(anim_hit_bullet, false, false, 60);
		pModel_->SetAnimation(m_animNo, false, false);
		m_updateFunc = &Enemy::UpdateHitBullet;
	}
	else
	{
		// ���S�A�j���[�V�����Ɉڍs
		m_animNo = anim_dead;
		pModel_->ChangeAnimation(anim_dead, false, false, 4);
		m_updateFunc = &Enemy::UpdateDead;
	}
}

bool Enemy::IsPlayerFront() const
{
	// ���ݓG�������Ă�������̃x�N�g���𐶐�����
	MATRIX enemyRotMtx = MGetRotY(m_angle);
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// �G����v���C���[�ւ̃x�N�g��
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), m_pos);
	toPlayer = VNorm(toPlayer);

	// ���ς���p�x�����߂�
	float dot = VDot(dir, toPlayer);
	float angle = acosf(dot);

	return (angle < view_angle);
}

void Enemy::UpdateToPlayer()
{
	m_damageFrame--;
	if (m_damageFrame < 0) m_damageFrame = 0;

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
	// �G����v���C���[�ւ̃x�N�g��(���K��)
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), m_pos);
	toPlayer = VNorm(toPlayer);

	m_pos = VAdd(m_pos, VScale(toPlayer, to_player_speed));
#endif
	pModel_->SetPos(m_pos);
	pModel_->SetRot(VGet(0.0f, m_angle, 0.0f));
}

void Enemy::UpdateToFront()
{
	m_damageFrame--;
	if (m_damageFrame < 0) m_damageFrame = 0;

	pModel_->Update();

	// ���ݓG�������Ă�������̃x�N�g���𐶐�����
	MATRIX enemyRotMtx = MGetRotY(m_angle);
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// �ړ����x�𔽉f������
	VECTOR vec = VScale(dir, to_front_speed);

	// �ړ�������
	m_pos = VAdd(m_pos, vec);

	m_frameCount++;
	if (m_frameCount >= 2 * 60)
	{
		m_rotSpeed = static_cast<float>(GetRand(250)) * 0.0001f;
		m_rotSpeed += 0.025;
		if (GetRand(1)) m_rotSpeed *= -1.0f;

		m_updateFunc = &Enemy::UpdateTurn;
		m_frameCount = 0;
	}

	pModel_->SetPos(m_pos);
	pModel_->SetRot(VGet(0.0f, m_angle, 0.0f));
}

void Enemy::UpdateTurn()
{
	m_damageFrame--;
	if (m_damageFrame < 0) m_damageFrame = 0;

	pModel_->Update();

	m_angle += m_rotSpeed;
	m_frameCount++;
	if (m_frameCount >= 30)
	{
		if (IsPlayerFront())
		{
			m_updateFunc = &Enemy::UpdateToPlayer;
			m_frameCount = 0;
		}
		else
		{
			m_updateFunc = &Enemy::UpdateToFront;
			m_frameCount = 0;
		}
	}

	pModel_->SetPos(m_pos);
	pModel_->SetRot(VGet(0.0f, m_angle, 0.0f));
}

void Enemy::UpdateHitBullet()
{
	m_damageFrame--;
	if (m_damageFrame < 0) m_damageFrame = 0;

	assert(m_animNo == anim_hit_bullet);
	pModel_->Update();

	if (pModel_->IsAnimEnd())
	{
		// �ҋ@�A�j���ɕύX����
		m_animNo = walk_anim;
		pModel_->ChangeAnimation(walk_anim, true, true, 4);

		// Update��ҋ@��
		m_updateFunc = &Enemy::UpdateToPlayer;
	}
}

void Enemy::UpdateDead()
{
	m_frameCount++;
	assert(m_animNo == anim_dead);
	pModel_->Update();

	if (pModel_->IsAnimEnd() && m_frameCount > 120)
	{
		isDead_ = true;
		m_frameCount = 0;
	}
}
