#include "Tower.h"
#include "../Model.h"
#include "../Collision.h"
#include "../Object/EnemyManager.h"
#include "../Object/EnemyBase.h"
#include "../StageManager.h"

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

	// タワーの拡大率
	constexpr float tower_scale = 0.4f;

	// 初期位置
	constexpr VECTOR stage_1_init_pos{ 6000.0f, -250.0f, 3000.0f };

	//// チェックポイント ////
	// チェックポイントの数
	constexpr int check_point_num = 4;
	 
	// ステージ１
	constexpr VECTOR stage_1_check_pos_1{ 980.0f, -291.0f, 2998.0f };
	constexpr VECTOR stage_1_check_pos_2{ 1934.0f, 4.0f, -4257.0f };
	constexpr VECTOR stage_1_check_pos_3{ -3996.0f, -52.0f, 695.0f };
	constexpr VECTOR stage_1_check_pos_4{ -4665.0f, -293.0f, -6190.0f};
}

Tower::Tower(StageManager* pStageManager) :
	pStageManager_(pStageManager),
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
		pos_ = stage_1_init_pos;
		checkPointPos1_ = stage_1_check_pos_1;
		checkPointPos2_ = stage_1_check_pos_2;
		checkPointPos3_ = stage_1_check_pos_3;
		checkPointPos4_ = stage_1_check_pos_4;
	}
	hp_ = max_hp;
	colRadius_ = 100.0f;
	checkPoint_ = check_point1;
	pModel_ = std::make_shared<Model>(adress);
	pModel_->SetUseCollision(true, true);
	pModel_->SetScale(VGet(tower_scale, tower_scale, tower_scale));
	pModel_->SetPos(pos_);
}

Tower::~Tower()
{
}

void Tower::Update()
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	CheckPointSet();
	HeadToDestination(checkPointPos_);

	if (checkPoint_ == goal)
	{
		isGoal_ = true;
		isMove_ = false;
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
	DrawFormatString(20, 500, 0x000000, "%d", checkPoint_);
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

void Tower::HeadToDestination(VECTOR checkPointPos)
{
	vec_ = VGet(0, 0, 0);
	if (isMove_)
	{
		// 敵から目標へのベクトルを求める
		VECTOR toTarget_ = VSub(checkPointPos, pos_);

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
	float dist = VSize(VSub(pos_, checkPointPos));
	if (dist < (colRadius_ + 50.0f))
	{
		if (!IsEnemyEnabled() && checkPoint_ <= check_point_num)
		{
			isMove_ = true;
			checkPoint_++;
		//	pEnemyManager_->Create(checkPoint_);
		}
		else 
		{
			isMove_ = false;
		}
	}
}

bool Tower::IsEnemyEnabled()
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
	}
}
