#include "Tower.h"
#include "../Model.h"
#include "../Collision.h"

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
	constexpr VECTOR check_pos_1{4316.818359, -68.738754, 3061.866211};
	constexpr VECTOR check_pos_2{};
	constexpr VECTOR check_pos_3{};
	constexpr VECTOR check_pos_4{};
	constexpr VECTOR check_pos_5{};
}

Tower::Tower() :
	pos_(VGet(0, 0, 0)),
	hp_(0),
	damageFrame_(0),
	colRadius_(0.0f),
	isDead_(false),
	pCollision_(nullptr),
	angle_(0.0f),
	isMove_(true)
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

	if(isMove_)
	{
		// �`�F�b�N�|�C���g�̓����蔻��
		float dist = VSize(VSub(pos_, check_pos_1));
		if (dist < (colRadius_ +  50.0f))
		{
			isMove_ = false;
		}

		// �G����ڕW�ւ̃x�N�g�������߂�
		VECTOR toTarget_ = VSub(check_pos_1, pos_);

		// �p�x�̎擾
		angle_ = static_cast<float>(atan2(toTarget_.x, toTarget_.z));

		// ���K��
		toTarget_ = VNorm(toTarget_);

		// �ړ����x�̔��f
		VECTOR vec = VScale(toTarget_, to_goal_speed);

		// �����蔻����s���A���̌��ʂɂ���Ĉړ�
		pos_ = pCollision_->Colision(pModel_->GetModelHandle(), isMove_, false, pos_, vec, Collision::Chara::tower);

		// �ʒu���W�̐ݒ�
		pModel_->SetPos(pos_);

		// �����Ă�������̐ݒ�
		pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));
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
