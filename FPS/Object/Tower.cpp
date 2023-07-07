#include "Tower.h"
#include "../Model.h"
#include "../Collision.h"
#include "../Object/EnemyManager.h"
#include "../Object/EnemyBase.h"

namespace
{
	// タワーのアドレス
	const char* const adress = "Data/Model/tower.mv1";

	// 最大HP
	constexpr int max_hp = 100;

	// ダメージを受けた時の無敵時間
	constexpr int invincible_time = 60;

	// HPバー
	constexpr int hp_bar_width = 500;	// HPバーの横幅
	constexpr int hp_bar_height = 50;	// HPバーの縦幅
	constexpr int hp_bar_x_pos = 1500;	// HPバーのx座標
	constexpr int hp_bar_y_pos = 100;	// HPバーのy座標

	// 移動速度
	constexpr float to_goal_speed = 5.0f;

	// 初期位置
	constexpr VECTOR init_pos{ 6000.0f, -250.0f, 3000.0f };

	// チェックポイント
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
	// ダメージ処理
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

	// 最大HPに対する現在のHPの割合を計算する
	float hpRate = static_cast<float>(hp_) / static_cast<float>(max_hp);

	// HPバーの長さを計算する
	int barWidth = static_cast<int>(hp_bar_width * hpRate);

	// HPバーの土台(赤)
//	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos + hp_bar_width / 2, hp_bar_y_pos + hp_bar_height, 0xff0000, true);

	// 現在のHP(緑)
	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos - hp_bar_width / 2 + barWidth, hp_bar_y_pos + hp_bar_height, 0x00ff00, true);

	// HPの枠
	DrawBox(hp_bar_x_pos - hp_bar_width / 2, hp_bar_y_pos, hp_bar_x_pos + hp_bar_width / 2, hp_bar_y_pos + hp_bar_height, 0x000000, false);

	DrawString(1250, 80, "TowerのHP", 0x000000);

	DrawFormatString(1100, 80, 0x000000, "%d", hp_);

	DrawFormatString(20, 400, 0x000000, "%f, %f, %f", pos_.x, pos_.y, pos_.z);
}

void Tower::OnDamage(int damage)
{
	// ダメージ処理
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
		// 敵から目標へのベクトルを求める
		VECTOR toTarget_ = VSub(checkPoint, pos_);

		// 角度の取得
		angle_ = static_cast<float>(atan2(toTarget_.x, toTarget_.z));

		// 正規化
		toTarget_ = VNorm(toTarget_);

		// 移動速度の反映
		vec_ = VScale(toTarget_, to_goal_speed);
	}
	
	// 当たり判定を行い、その結果によって移動
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), isMove_, false, pos_, vec_, Collision::Chara::tower);

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	// チェックポイントの当たり判定
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
