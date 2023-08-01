#include "Bee.h"
#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
#include "../Collision.h"
#include "Tower.h"
#include "../EnemyShotFactory.h"
#include "../SoundManager.h"
#include <cmath>
#include <cassert>

namespace
{
	// �t�@�C���p�X
	const char* const bee_adress = "Data/Model/beeCol.mv1";

	// �G�L�����N�^�[�̌����Ă������
	constexpr VECTOR enemy_dir{ 0.0f, 0.0f, -1.0f };

	// �G�L�����N�^�[�̈ړ����x
	constexpr float to_front_speed = 4.0f;
	constexpr float to_player_speed = 4.0f;

	// �G�L�����N�^�[�̎���p
	constexpr float view_angle = 90.0f * DX_PI_F / 180.0f;

	// �A�j���[�V�����ԍ�
	constexpr int walk_anim_no = 1;
	constexpr int ondamage_anim_no = 2;
	constexpr int dead_anim_no = 0;
	constexpr int attack_anim_no = 3;

	// �����蔼�a�̃T�C�Y
	constexpr float collision_radius = 70.0f;

	// �ő�HP
	constexpr int max_hp = 50;

	// �_���[�W�󂯂����̖��G����
	constexpr int invincible_time = 10;

	// �v���C���[�ɍU�����鋗��
	constexpr float player_attack_distance = 2000.0f;

	// �^���[�ɍU�����鋗��
	constexpr float tower_attack_distance = 2000.0f;

	// �ڕW������������
	constexpr float lost_distance = 4000.0f;

	// �U���̍Ďg�p�܂őҋ@�t���[����
	constexpr int attack_wait_time = 20;

	// ���m�͈�
	constexpr float detection_range = 1000.0f;
}

Bee::Bee(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, VECTOR pos, bool isMove, int handle)
{
	pPlayer_ = pPlayer;
	pTower_ = pTower;
	pCollision_ = pCollision;
	pEnemyShotFactory_ = pEnemyShotFactory;

	pos_ = pos;
	animNo_ = walk_anim_no;
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
	discoverAnimNo_ = -1;
	detectionRange_ = detection_range;
	handle_ = handle;

	// �G����ڕW�ւ̃x�N�g�������߂�
	toTargetVec_ = VSub(pPlayer_->GetPos(), pos_);

	// �p�x�̎擾
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	if (isMove)
	{
		updateFunc_ = &Bee::UpdateToFront;
	}
	else
	{
		updateFunc_ = &Bee::UpdateToIdle;
	}

	// 3D���f���̐���
	pModel_ = std::make_shared<Model>(bee_adress);
	pModel_->SetAnimation(animNo_, true, true);
	pModel_->SetUseCollision(true, true);
	pModel_->SetPos(pos);
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));
	pModel_->Update();
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

void Bee::Tracking(VECTOR pos, int target, float attackDistance)
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �G����ڕW�ւ̃x�N�g�������߂�
	toTargetVec_ = VSub(pos, pos_);

	// Y���͒ǂ������Ȃ�
	toTargetVec_ = VGet(toTargetVec_.x, 0.0f, toTargetVec_.z);

	// �p�x�̎擾
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	// ���K��
	toTargetVec_ = VNorm(toTargetVec_);

	// �ړ����x�̔��f
	VECTOR vec = VScale(toTargetVec_, to_player_speed);

	// �t�B�[���h�Ƃ̓����蔻����s���A���̌��ʂɂ���Ĉړ�
	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), true, false, false, pos_, vec, Collision::Chara::bee, collision_radius);

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
			updateFunc_ = &Bee::UpdateAttackToPlayer;
			break;
		case tower:
			updateFunc_ = &Bee::UpdateAttackToTower;
			break;
		}
		frameCount_ = 0;
	}
	if (target == player && distans > lost_distance)
	{
		updateFunc_ = &Bee::UpdateTrackingToTower;
	}

	// �v���C���[������ł���ꍇ��ǂ�Ȃ�
	if (pPlayer_->GetIsDead())
	{
		updateFunc_ = &Bee::UpdateToFront;
		frameCount_ = 0;
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	Sound();
}

void Bee::Attacking(VECTOR pos, int target, float attackDistance)
{
	assert(animNo_ == attack_anim_no);

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �G����ڕW�ւ̃x�N�g�������߂�
	toTargetVec_ = VSub(pos, pos_);

	// �p�x�̎擾
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	// �^���[�̑�����_������Y�����v���X
	if(target == tower)
		toTargetVec_ = VGet(toTargetVec_.x, toTargetVec_.y + 500.0f, toTargetVec_.z);

	// �v���C���[�܂ł̋���
	float distans = VSize(toTargetVec_);

	// ���K��
	toTargetVec_ = VNorm(toTargetVec_);

	// �V���b�g�̃X�s�[�h��������
	toTargetVec_ = VScale(toTargetVec_, 20.0f);

	// 
	if (attackWaitTimer_++ % attack_wait_time == 0)
	{
		pEnemyShotFactory_->ShootStart(pos_, toTargetVec_, target);
	}

	// �v���C���[�������̋�������Ă�����v���C���[��ǂ�������
	if (attackDistance < distans)
	{
		// �A�j���[�V�����ݒ�
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		// update��ύX
		switch (target)
		{
		case player:
			updateFunc_ = &Bee::UpdateTrackingToPlayer;
			break;
		case tower:
			updateFunc_ = &Bee::UpdateTrackingToTower;
		}
		frameCount_ = 0;
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	Sound();
}

void Bee::UpdateToIdle()
{
	frameCount_++;
	if (!targetDiscover_ && frameCount_ > 120)
	{
		// �^���[����������v���C���[��ǂ�������
		if (IsTargetDetection(pTower_->GetPos(), pTower_->GetColRadius()) && !pPlayer_->GetIsDead())
		{
			target_ = tower;
		}
		// �v���C���[����������v���C���[��ǂ�������
		else if (IsTargetDetection(pPlayer_->GetPos(), pPlayer_->GetCollisionRadius()) && !pPlayer_->GetIsDead())
		{
			target_ = player;
		}
		animNo_ = discoverAnimNo_;
	//	pModel_->ChangeAnimation(animNo_, false, false, 4);
		targetDiscover_ = true;
		frameCount_ = 0;
	}
	else if (targetDiscover_)
	{
	//	if (pModel_->IsAnimEnd())
		if(frameCount_ > 20)
		{
			targetDiscover_ = false;
			animNo_ = walk_anim_no;
			pModel_->ChangeAnimation(animNo_, true, false, 4);
			frameCount_ = 0;
			if (target_ == tower)
			{
				updateFunc_ = &Bee::UpdateTrackingToTower;
			}
			else if (target_ == player)
			{
				updateFunc_ = &Bee::UpdateTrackingToPlayer;
			}
		}
	}
	// �A�j���[�V�����X�V����
	pModel_->Update();

	Sound();
}

void Bee::UpdateTrackingToPlayer()
{
	Tracking(pPlayer_->GetPos(), player, player_attack_distance);
}

void Bee::UpdateTrackingToTower()
{
	Tracking(pTower_->GetPos(), tower, tower_attack_distance);
}

void Bee::UpdateAttackToPlayer()
{
	Attacking(pPlayer_->GetPos(), player, player_attack_distance);
}

void Bee::UpdateAttackToTower()
{
	Attacking(pTower_->GetPos(), tower, tower_attack_distance);
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
	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), true, false, false, pos_, vec, Collision::Chara::bee, collision_radius);

	frameCount_++;
	if (frameCount_ >= 2 * 60)
	{
		// �^���[����������v���C���[��ǂ�������
		if (IsTargetDetection(pTower_->GetPos(), pTower_->GetColRadius()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateTrackingToTower;
			frameCount_ = 0;
		}
		// �v���C���[����������v���C���[��ǂ�������
		else if (IsTargetDetection(pPlayer_->GetPos(), pPlayer_->GetCollisionRadius()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateTrackingToPlayer;
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
			updateFunc_ = &Bee::UpdateTurn;
			frameCount_ = 0;
		}
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �A�j���[�V�����X�V����
	pModel_->Update();

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ , 0.0f));

	Sound();
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
		// �^���[����������v���C���[��ǂ�������
		if (IsPlayerFront(pTower_->GetPos()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateTrackingToTower;
			frameCount_ = 0;
		}
		// �v���C���[����������v���C���[��ǂ�������
		else if (IsPlayerFront(pPlayer_->GetPos()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateTrackingToPlayer;
			frameCount_ = 0;
		}
		else
		{
			updateFunc_ = &Bee::UpdateToFront;
			frameCount_ = 0;
		}
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �A�j���[�V�����X�V����
	pModel_->Update();
	
	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));

	Sound();
}

void Bee::UpdateHitDamage()
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
		updateFunc_ = &Bee::UpdateTrackingToPlayer;
	}
}

void Bee::Sound()
{
	auto& soundManager = SoundManager::GetInstance();
	if (!soundManager.CheckMusic("bee"))
	{
		soundManager.Play3D("bee", pos_, 1500.0f, false);
	}
}