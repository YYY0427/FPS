#include "Enemy.h"
#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
#include "Tower.h"
#include "../Collision.h"
#include <cassert>

namespace
{
	// �t�@�C���p�X
	const char* const enemy_adress = "Data/Model/enemyCol.mv1";

	// �G�L�����N�^�[�̌����Ă������
	constexpr VECTOR enemy_dir{ 0.0f, 0.0f, -1.0f };

	// �G�L�����N�^�[�̈ړ����x
	constexpr float to_front_speed = 8.0f;
	constexpr float to_player_speed = 8.0f;

	// �G�L�����N�^�[�̎���p
	constexpr float view_angle = 30.0f * DX_PI_F / 180.0f;

	// �A�j���[�V�����ԍ�
	constexpr int walk_anim_no = 8;
	constexpr int ondamage_anim_no = 4;
	constexpr int dead_anim_no = 3;
	constexpr int attack_anim_no = 0;
	constexpr int idle_anim_no = 2;

	// �����蔼�a�̃T�C�Y
	constexpr float collision_radius = 70.0f;

	// �ő�HP
	constexpr int max_hp = 30;

	// �_���[�W�󂯂����̖��G����
	constexpr int invincible_time = 10;

	// �v���C���[�ɍU�����鋗��
	constexpr float player_attack_distance = 140.0f;

	// �^���[�ɍU�����鋗��
	constexpr float tower_attack_distance = 250.0f;

	// �ڕW������������
	constexpr float lost_distance = 2000.0f;

	// �U���̍Ďg�p�܂őҋ@�t���[����
	constexpr int attack_wait_time = 20;

	// ���m�͈�
	constexpr float detection_range = 1500.0f;
}

Enemy::Enemy(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, VECTOR pos, bool isMove)
{
	pPlayer_ = pPlayer;
	pTower_ = pTower;
	pCollision_ = pCollision;
	pEnemyShotFactory_ = pEnemyShotFactory;

	pos_ = pos;
	frameCount_ = 0;
	rotSpeed_ = 0;
	sHp_.hp_ = max_hp;
	damageFrame_ = 0;
	isDead_ = false;
	colRadius_ = collision_radius;
	sHp_.maxHp_ = max_hp;
	viewAngle_ = view_angle;
	dir_ = enemy_dir;
	deadDisappearTime_ = 120;
	sHp_.hpUIDrawY_ = 30.0f;
	deadAnimNo_ = dead_anim_no;
	detectionRange_ = detection_range;

	// �G����ڕW�ւ̃x�N�g�������߂�
	toTargetVec_ = VSub(pPlayer_->GetPos(), pos_);

	// �p�x�̎擾
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	if (isMove)
	{
		animNo_ = walk_anim_no;
		updateFunc_ = &Enemy::UpdateToFront;
	}
	else
	{
		animNo_ = idle_anim_no;
		updateFunc_ = &Enemy::UpdateToIdle;	
	}

	// 3D���f���̐���
	pModel_ = std::make_shared<Model>(enemy_adress);
	pModel_->SetAnimation(animNo_, true, true);
	pModel_->SetUseCollision(true);
	pModel_->SetPos(pos);
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));
	pModel_->Update();
}

Enemy::~Enemy()
{

}

void Enemy::Update()
{
	(this->*updateFunc_)();
}

void Enemy::OnDamage(int damage)
{
	// �_���[�W����
	if (damageFrame_ > 0)	return;
	sHp_.hp_ -= damage;
	damageFrame_ = invincible_time;

	// HP������΃_���[�W�A�j���[�V�����Ɉڍs
	// HP���Ȃ���Ύ��S�A�j���[�V�����Ɉڍs
	if (sHp_.hp_ > 0)
	{
		// �A�j���[�V�����ݒ�
		animNo_ = ondamage_anim_no;
		pModel_->ChangeAnimation(animNo_, false, false, 4);

		// update�ύX
		updateFunc_ = &Enemy::UpdateHitDamage;
	}
	else
	{
		// ���S�A�j���[�V�����Ɉڍs
		animNo_ = dead_anim_no;
		pModel_->ChangeAnimation(animNo_, false, false, 4);
		updateFunc_ = &Enemy::UpdateDead;
	}
}

void Enemy::Tracking(VECTOR pos, int target, float attackDistance)
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �G����ڕW�ւ̃x�N�g�������߂�
	toTargetVec_ = VSub(pos, pos_);

	// �p�x�̎擾
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	// ���K��
	toTargetVec_ = VNorm(toTargetVec_);

	// �ړ����x�̔��f
	VECTOR vec = VScale(toTargetVec_, to_player_speed);

	
	// �^�[�Q�b�g�܂ł̋���
	float distans = VSize(VSub(pos, pos_));

	// �ڕW�܂ł܂ł̋��������苗���ȓ��Ȃ�U���A�j���[�V�����Ɉڍs
	if (distans < attackDistance)
	{
		// �A�j���[�V�����ݒ�
		animNo_ = attack_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		// ���݌������Ă���Ώۂɂ���čU���Ώۂ�����
		switch (target)
		{
		case player:
			updateFunc_ = &Enemy::UpdateAttackToPlayer;
			break;
		case tower:
			updateFunc_ = &Enemy::UpdateAttackToTower;
			break;
		}
		frameCount_ = 0;
	}
	if (target == player && distans > lost_distance)
	{
		updateFunc_ = &Enemy::UpdateTrackingToTower;
	}

	// �v���C���[������ł���ꍇ��ǂ�Ȃ�
	if (pPlayer_->GetIsDead())
	{
		updateFunc_ = &Enemy::UpdateToFront;
		frameCount_ = 0;
	}

	// �����蔻����s���A���̌��ʂɂ���Ĉړ�
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), true, false, true, pos_, vec, Collision::Chara::enemy, collision_radius);

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	MV1RefreshCollInfo(pModel_->GetModelHandle(), -1);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	// �A�j���[�V�����X�V����
	pModel_->Update();
}

void Enemy::Attacking(VECTOR pos, int target, float attacDistance)
{
	assert(animNo_ == attack_anim_no);

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �w�肳�ꂽ�t���[����1��U������
	if (attackWaitTimer_++ % attack_wait_time == 0)
	{
		// �U��
		isAttack_ = true;
	}
	else
	{
		isAttack_ = false;
	}

	// �G����ڕW�ւ̃x�N�g�������߂�
	toTargetVec_ = VSub(pos, pos_);

	// �p�x�̎擾
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	// �^�[�Q�b�g�܂ł̋���
	float distans = VSize(toTargetVec_);

	// �^�[�Q�b�g�������̋�������Ă�����v���C���[��ǂ�������
	if (attacDistance < distans)
	{
		// �A�j���[�V�����ݒ�
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		// update��ύX
		switch (target)
		{
		case player:
			updateFunc_ = &Enemy::UpdateTrackingToPlayer;
			break;
		case tower:
			updateFunc_ = &Enemy::UpdateTrackingToTower;
		}
		frameCount_ = 0;
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	// �A�j���[�V�����X�V����
	pModel_->Update();
}

void Enemy::UpdateToIdle()
{
	// �^���[����������v���C���[��ǂ�������
	if (IsTargetDetection(pTower_->GetPos(), pTower_->GetColRadius()) && !pPlayer_->GetIsDead())
	{
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		updateFunc_ = &Enemy::UpdateTrackingToTower;
		frameCount_ = 0;
	}
	// �v���C���[����������v���C���[��ǂ�������
	else if (IsTargetDetection(pPlayer_->GetPos(), pPlayer_->GetColRadius()) && !pPlayer_->GetIsDead())
	{
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		updateFunc_ = &Enemy::UpdateTrackingToPlayer;
		frameCount_ = 0;
	}
	
	// �t�B�[���h�Ƃ̓����蔻����s���A���̌��ʂɂ���Ĉړ�
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), false , false, true, pos_, VGet(0.0f, 0.0f, 0.0f), Collision::Chara::enemy, collision_radius);

	pModel_->SetPos(pos_);

	// �A�j���[�V�����X�V����
	pModel_->Update();
}

void Enemy::UpdateTrackingToPlayer()
{
	Tracking(pPlayer_->GetPos(), player, player_attack_distance);
}

void Enemy::UpdateTrackingToTower()
{
	Tracking(pTower_->GetPos(), tower, tower_attack_distance);
}

void Enemy::UpdateToFront()
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
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), true, false, true, pos_, vec, Collision::Chara::enemy, collision_radius);

	frameCount_++;
	if (frameCount_ >= 2 * 60)
	{
		// �^���[����������v���C���[��ǂ�������
		if (IsTargetDetection(pTower_->GetPos(), pTower_->GetColRadius()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Enemy::UpdateTrackingToTower;
			frameCount_ = 0;
		}
		// �v���C���[����������v���C���[��ǂ�������
		else if (IsTargetDetection(pPlayer_->GetPos(), pPlayer_->GetColRadius()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Enemy::UpdateTrackingToPlayer;
			frameCount_ = 0;
		}
		// ������Ȃ��������]����
		else
		{
			// ��]����p�x�������_���Ōv�Z
			rotSpeed_ = static_cast<float>(GetRand(250)) * 0.0001f;
			rotSpeed_ += 0.025f;
			if (GetRand(1)) rotSpeed_ *= -1.0f;

			// udpate��ύX
			updateFunc_ = &Enemy::UpdateTurn;
			frameCount_ = 0;
		}	
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));

	// �A�j���[�V�����X�V����
	pModel_->Update();
}

void Enemy::UpdateAttackToPlayer()
{
	Attacking(pPlayer_->GetPos(), player, player_attack_distance);
}

void Enemy::UpdateAttackToTower()
{
	Attacking(pTower_->GetPos(), tower, tower_attack_distance);
}

void Enemy::UpdateTurn()
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// ��]�̉��Z
	angle_ += rotSpeed_;

	frameCount_++;
	if (frameCount_ >= 30)
	{
		if (IsPlayerFront(pPlayer_->GetPos()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Enemy::UpdateTrackingToPlayer;
			frameCount_ = 0;
		}
		else
		{
			updateFunc_ = &Enemy::UpdateToFront;
			frameCount_ = 0;
		}
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ , 0.0f));
}

void Enemy::UpdateHitDamage()
{
	assert(animNo_ == ondamage_anim_no);

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
		updateFunc_ = &Enemy::UpdateTrackingToPlayer;
	}
}