#include "Tower.h"
#include "../Model.h"

namespace
{
	// 最大HP
	constexpr int max_hp = 50;

	// ダメージを受けた時の無敵時間
	constexpr int invincible_time = 60;

	// HPバー
	constexpr int hp_bar_width = 500;	// HPバーの横幅
	constexpr int hp_bar_height = 50;	// HPバーの立幅
	constexpr int hp_bar_x_pos = 1500;	// HPバーのx座標
	constexpr int hp_bar_y_pos = 100;	// HPバーのy座標
}

Tower::Tower() :
	pos_(VGet(0, 0, 0)),
	hp_(0),
	damageFrame_(0)
{
}

Tower::~Tower()
{
}

void Tower::Init()
{
//	pModel_ = std::make_shared<Model>();
	hp_ = max_hp;
	pos_ = VGet(100, 100, 0);
}

void Tower::Update()
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;
}

void Tower::Draw()
{
//	pModel_->Draw();

	// 最大HPに対する現在のHPの割合を計算する
	float hpRate = static_cast<float>(hp_) / static_cast<float>(max_hp);

	// HPバーの長さを計算する
	float barWidth = hp_bar_width * hpRate;

	// 現在のHP(緑)
	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos - hp_bar_width / 2 + barWidth, hp_bar_y_pos + hp_bar_height, 0x00ff00, true);

	// HPの枠
	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos + hp_bar_width / 2, hp_bar_y_pos + hp_bar_height, 0xffffff, false);

	DrawSphere3D(pos_, 100.0f, 50, 0xff0000, 0xff0000, true);

	DrawString(1250, 80, "TowerのHP", 0xffffff);
}

void Tower::Ondamage(int damage)
{
	// ダメージ処理
	if (damageFrame_ > 0)	return;
	hp_ -= damage;
	damageFrame_ = invincible_time;
}
