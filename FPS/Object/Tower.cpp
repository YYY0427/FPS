#include "Tower.h"
#include "../Model.h"
#include "../Collision.h"
#include "../Object/EnemyManager.h"
#include "../Object/EnemyBase.h"
#include "../StageManager.h"
#include "../ObstacleManager.h"
#include "../Obstacle.h"

namespace
{
	// �^���[�̃A�h���X
	const char* const adress = "Data/Model/tower.mv1";

	// �ő�HP
	constexpr int max_hp = 10000;

	// �_���[�W���󂯂����̖��G����
	constexpr int invincible_time = 0;

	// HP�o�[
	constexpr int hp_bar_width = 500;	// HP�o�[�̉���
	constexpr int hp_bar_height = 50;	// HP�o�[�̏c��
	constexpr int hp_bar_x_pos = 1500;	// HP�o�[��x���W
	constexpr int hp_bar_y_pos = 100;	// HP�o�[��y���W

	// �ړ����x
	constexpr float to_goal_speed = 5.0f;

	 // ���f���̊g�嗦
	constexpr float model_scale = 0.4f;
	 
	// ���f����n�ʂ���ǂꂾ����������
	constexpr float ground_far = 150.0f;

	// �X�e�[�W�P
	constexpr VECTOR stage_1_start_pos{ 6959.0f, -100.0f, 2370.0f };
	constexpr VECTOR stage_1_check_pos_1{ 4690.0f, -331.0f, 2146.0f };
	constexpr VECTOR stage_1_check_pos_2{ 4614.0f, -291.0f, 351.0f };
	constexpr VECTOR stage_1_check_pos_3{ 1985.0f, -228.0f, -1185.0f };
	constexpr VECTOR stage_1_check_pos_4{ 1262.0f, -7.0f, -2830.0f};
	constexpr VECTOR stage_1_check_pos_5{ -286.0f, -329.0f, -2560.0f};
	constexpr VECTOR stage_1_check_pos_6{ -1622.0f, -372.0f, -807.0f };
	constexpr VECTOR stage_1_check_pos_7{ -2724.0f, -343.0f, -2045.0f };
	constexpr VECTOR stage_1_check_pos_8{ -1367.0f, -290.0f, -4390.0f };
	constexpr VECTOR stage_1_check_pos_9{ -3572.0f, -324.0f, -5984.0f };
}

Tower::Tower(StageManager* pStageManager, std::shared_ptr<ObstacleManager> pObstacleManager) :
	pStageManager_(pStageManager),
	pObstacleManager_(pObstacleManager),
	pos_(VGet(0, 0, 0)),
	hp_(0),
	damageFrame_(0),
	colRadius_(0.0f),
	isDead_(false),
	pCollision_(nullptr),
	angle_(0.0f),
	isMove_(true),
	checkPointPos_(VGet(0, 0, 0)),
	checkPoint_(0),
	vec_(VGet(0, 0, 0)),
	isGoal_(false)
{
	switch (pStageManager_->GetAnyStage())
	{
	case 0:
		pos_ = stage_1_start_pos;
		checkPointPos1_ = stage_1_check_pos_1;
		checkPointPos2_ = stage_1_check_pos_2;
		checkPointPos3_ = stage_1_check_pos_3;
		checkPointPos4_ = stage_1_check_pos_4;
		checkPointPos5_ = stage_1_check_pos_5;
		checkPointPos6_ = stage_1_check_pos_6;
		checkPointPos7_ = stage_1_check_pos_7;
		checkPointPos8_ = stage_1_check_pos_8;
		checkPointPos9_ = stage_1_check_pos_9;
	}
	hp_ = max_hp;
	colRadius_ = 50.0f;
	checkPoint_ = check_point1;
	pModel_ = std::make_shared<Model>(adress);
	pModel_->SetUseCollision(true);
	pModel_->SetScale(VGet(model_scale, model_scale, model_scale));
	pModel_->SetPos(pos_);
	pModel_->Update();
}

Tower::~Tower()
{
}

void Tower::Update()
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	CheckPointSet();
	HeadToDestination(VGet(checkPointPos_.x, pos_.y, checkPointPos_.z));

	if (checkPoint_ == goal)
	{
		isGoal_ = true;
		isMove_ = false;
	}

	pModel_->Update();
}

void Tower::Draw()
{
	// �����Ă���悤�Ɍ����邽�߂Ɏ����W�Ƃ͈Ⴄ���W�ŕ\������
	pModel_->SetPos(VGet(pos_.x, pos_.y + ground_far, pos_.z));		// �����W������������
	pModel_->Draw();												// �\��	
	pModel_->SetPos(VGet(pos_.x, pos_.y - ground_far, pos_.z));		// �����W�ɖ߂�

	// �ő�HP�ɑ΂��錻�݂�HP�̊������v�Z����
	float hpRate = static_cast<float>(hp_) / static_cast<float>(max_hp);

	// HP�o�[�̒������v�Z����
	int barWidth = static_cast<int>(hp_bar_width * hpRate);

	// HP�o�[�̓y��(��)
//	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos + hp_bar_width / 2, hp_bar_y_pos + hp_bar_height, 0xff0000, true);

	// ���݂�HP(��)
	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos - hp_bar_width / 2 + barWidth, hp_bar_y_pos + hp_bar_height, 0x00ff00, true);

	// HP�̘g
	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos + hp_bar_width / 2, hp_bar_y_pos + hp_bar_height, 0x000000, false);

#ifdef _DEBUG
	DrawFormatString(1100, 80, 0x000000, "towerHp = %d", hp_);
	DrawFormatString(20, 400, 0x000000, "towerPos = %f, %f, %f", pos_.x, pos_.y, pos_.z);
	DrawFormatString(20, 500, 0x000000, "checkPoint = %d", checkPoint_);
#endif
}

void Tower::OnDamage(int damage)
{
	// �_���[�W����
	if (damageFrame_ > 0)	return;
	hp_ -= damage;
	damageFrame_ = invincible_time;
	if (hp_ <= 0)
	{
		isDead_ = true;
		hp_ = 0;
	}
}

void Tower::HeadToDestination(VECTOR checkPointPos)
{
	vec_ = VGet(0, 0, 0);
	if (isMove_)
	{
		// �G����ڕW�ւ̃x�N�g�������߂�
		VECTOR toTarget = VSub(checkPointPos, pos_);

		// �p�x�̎擾
		angle_ = static_cast<float>(atan2(toTarget.x, toTarget.z));

		// ���K��
		toTarget = VNorm(toTarget);

		// �ړ����x�̔��f
		vec_ = VScale(toTarget, to_goal_speed);
	}

	// �����蔻����s���A���̌��ʂɂ���Ĉړ�
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), isMove_, false, true, pos_, vec_, Collision::Chara::tower, colRadius_);

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	IsObstacleFront();

	// �`�F�b�N�|�C���g�̓����蔻��
	float dist = VSize(VSub(pos_, checkPointPos));
	if (dist < (colRadius_ + 50.0f))
	{
		if (checkPoint_ < goal)
		{
			checkPoint_++;
		//	pEnemyManager_->Create(checkPoint_);
		}
		else
		{
			isMove_ = false;
		}
	}
}

void Tower::IsObstacleFront()
{
	/*int cnt = 0;
	for (auto& enemy : pEnemyManager_->GetEnemies())
	{
		if (!enemy->GetDead())
		{
			cnt++;
		}
	}
	return (cnt > 0);*/
	isMove_ = true;
	for (auto& obj : pObstacleManager_->GetObstacles())
	{
		float dist = VSize(VSub(pos_, obj->GetPos()));
		if (dist < (colRadius_ + obj->GetTowerToCollsionRadius()))
		{
			isMove_ = false;
			break;
		}
	}
}

void Tower::CheckPointSet()
{
	switch (checkPoint_)
	{
	case check_point1:
		checkPointPos_ = checkPointPos1_;
		break;
	case check_point2:
		checkPointPos_ = checkPointPos2_;
		break;
	case check_point3:
		checkPointPos_ = checkPointPos3_;
		break;
	case check_point4:
		checkPointPos_ = checkPointPos4_;
		break;
	case check_point5:
		checkPointPos_ = checkPointPos5_;
		break;
	case check_point6:
		checkPointPos_ = checkPointPos6_;
		break;
	case check_point7:
		checkPointPos_ = checkPointPos7_;
		break;
	case check_point8:
		checkPointPos_ = checkPointPos8_;
		break;
	case check_point9:
		checkPointPos_ = checkPointPos9_;
		break;
	}
}
