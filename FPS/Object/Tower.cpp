#include "Tower.h"
#include "../Model.h"
#include "../Collision.h"
#include "../Object/EnemyManager.h"
#include "../Object/EnemyBase.h"

namespace
{
	// �^���[�̃A�h���X
	const char* const adress = "Data/Model/tower.mv1";

	// �ő�HP
	constexpr int max_hp = 100;

	// �_���[�W���󂯂����̖��G����
	constexpr int invincible_time = 60;

	// HP�o�[
	constexpr int hp_bar_width = 500;	// HP�o�[�̉���
	constexpr int hp_bar_height = 50;	// HP�o�[�̏c��
	constexpr int hp_bar_x_pos = 1500;	// HP�o�[��x���W
	constexpr int hp_bar_y_pos = 100;	// HP�o�[��y���W

	// �ړ����x
	constexpr float to_goal_speed = 5.0f;

	// �����ʒu
	constexpr VECTOR init_pos{ 6000.0f, -250.0f, 3000.0f };

	// �`�F�b�N�|�C���g
	constexpr VECTOR check_pos_1{ 4316.818359f, -68.738754f, 3061.866211f};
	constexpr VECTOR check_pos_2{ 2353.516846f, -185.86734f, 14.413342f };
	constexpr VECTOR check_pos_3{ 1389.046021f, -37.761520f, -3957.060059f};
	constexpr VECTOR check_pos_4{ -2108.958008f, -284.074921, -404.127014 };
	constexpr VECTOR check_pos_5{ -4665.145996, -293.110107, -6190.505859 };
}

Tower::Tower() :
	pos_(VGet(0, 0, 0)),
	hp_(0),
	damageFrame_(0),
	colRadius_(0.0f),
	isDead_(false),
	pCollision_(nullptr),
	angle_(0.0f),
	isMove_(true),
	checkPointNow_(VGet(0, 0, 0)),
	checkPoint_(0),
	vec_(VGet(0, 0, 0)),
	isGoal_(false)
{
}

Tower::~Tower()
{
}

void Tower::Init()
{
	pos_ = init_pos;
	hp_ = max_hp;
	colRadius_ = 100.0f;
	checkPoint_ = k_check_point1;
	pModel_ = std::make_shared<Model>(adress);
	pModel_->SetUseCollision(true, true);
	pModel_->SetScale(VGet(0.4f, 0.4f, 0.4f));
	pModel_->SetPos(pos_);
}

void Tower::Update()
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	CheckPointSet();
	HeadToDestination(checkPointNow_);

	if (checkPoint_ == 5)
	{
		isGoal_ = true;
	}

	pModel_->Update();
}

void Tower::Draw()
{
	pModel_->Draw();

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

	DrawString(1250, 80, "Tower��HP", 0x000000);

	DrawFormatString(1100, 80, 0x000000, "%d", hp_);

	DrawFormatString(20, 400, 0x000000, "%f, %f, %f", pos_.x, pos_.y, pos_.z);
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

void Tower::HeadToDestination(VECTOR checkPoint)
{
	vec_ = VGet(0, 0, 0);
	if (isMove_)
	{
		// �G����ڕW�ւ̃x�N�g�������߂�
		VECTOR toTarget_ = VSub(checkPoint, pos_);

		// �p�x�̎擾
		angle_ = static_cast<float>(atan2(toTarget_.x, toTarget_.z));

		// ���K��
		toTarget_ = VNorm(toTarget_);

		// �ړ����x�̔��f
		vec_ = VScale(toTarget_, to_goal_speed);
	}
	
	// �����蔻����s���A���̌��ʂɂ���Ĉړ�
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), isMove_, false, pos_, vec_, Collision::Chara::tower);

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	// �`�F�b�N�|�C���g�̓����蔻��
	float dist = VSize(VSub(pos_, checkPoint));
	if (dist < (colRadius_ + 50.0f))
	{
		if (!IsEnemyExists() && checkPoint_ < 5)
		{
			isMove_ = true;
			checkPoint_++;
		}
		else
		{
			isMove_ = false;
		}
	}
}

bool Tower::IsEnemyExists()
{
	int cnt = 0;
	for (auto& enemy : pEnemyManager_->GetEnemies())
	{
		if (!enemy->GetDead())
		{
			cnt++;
		}
	}
	return (cnt > 0);
}

void Tower::CheckPointSet()
{
	switch (checkPoint_)
	{
	case k_check_point1:
		checkPointNow_ = check_pos_1;
		break;
	case k_check_point2:
		checkPointNow_ = check_pos_2;
		break;
	case k_check_point3:
		checkPointNow_ = check_pos_3;
		break;
	case k_check_point4:
		checkPointNow_ = check_pos_4;
		break;
	case k_check_point5:
		checkPointNow_ = check_pos_5;
		break;
	}
}
