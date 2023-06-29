#include "Tower.h"
#include "../Model.h"

namespace
{
	// �ő�HP
	constexpr int max_hp = 100;

	// �_���[�W���󂯂����̖��G����
	constexpr int invincible_time = 60;

	// HP�o�[
	constexpr int hp_bar_width = 500;	// HP�o�[�̉���
	constexpr int hp_bar_height = 50;	// HP�o�[�̏c��
	constexpr int hp_bar_x_pos = 1500;	// HP�o�[��x���W
	constexpr int hp_bar_y_pos = 100;	// HP�o�[��y���W
}

Tower::Tower() :
	pos_(VGet(0, 0, 0)),
	hp_(0),
	damageFrame_(0),
	colRadius_(0.0f),
	isDead_(false)
{
}

Tower::~Tower()
{
}

void Tower::Init()
{
//	pModel_ = std::make_shared<Model>();
	hp_ = max_hp;
	colRadius_ = 100.0f;
	pos_ = VGet(100, 100, 0);
}

void Tower::Update()
{
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;
}

void Tower::Draw()
{
//	pModel_->Draw();

	// �ő�HP�ɑ΂��錻�݂�HP�̊������v�Z����
	float hpRate = static_cast<float>(hp_) / static_cast<float>(max_hp);

	// HP�o�[�̒������v�Z����
	int barWidth = hp_bar_width * hpRate;

	// HP�o�[�̓y��(��)
//	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos + hp_bar_width / 2, hp_bar_y_pos + hp_bar_height, 0xff0000, true);

	// ���݂�HP(��)
	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos - hp_bar_width / 2 + barWidth, hp_bar_y_pos + hp_bar_height, 0x00ff00, true);

	// HP�̘g
	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos + hp_bar_width / 2, hp_bar_y_pos + hp_bar_height, 0x000000, false);

	DrawSphere3D(pos_, colRadius_, 50.0f, 0xff0000, 0xff0000, true);

	DrawString(1250, 80, "Tower��HP", 0x000000);

	DrawFormatString(1100, 80, 0x000000, "%d", hp_);
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
