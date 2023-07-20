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
	// タワーのアドレス
	const char* const adress = "Data/Model/tower.mv1";

	// 最大HP
	constexpr int max_hp = 10000;

	// ダメージを受けた時の無敵時間
	constexpr int invincible_time = 0;

	// HPバー
	constexpr int hp_bar_width = 500;	// HPバーの横幅
	constexpr int hp_bar_height = 50;	// HPバーの縦幅
	constexpr int hp_bar_x_pos = 1500;	// HPバーのx座標
	constexpr int hp_bar_y_pos = 100;	// HPバーのy座標

	// 移動速度
	constexpr float to_goal_speed = 5.0f;

	 // モデルの拡大率
	constexpr float model_scale = 0.4f;
	 
	// モデルを地面からどれだけ浮かすか
	constexpr float ground_far = 150.0f;

	// ステージ１
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
	// ダメージ処理
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
	// 浮いているように見せるために実座標とは違う座標で表示する
	pModel_->SetPos(VGet(pos_.x, pos_.y + ground_far, pos_.z));		// 実座標を書き換える
	pModel_->Draw();												// 表示	
	pModel_->SetPos(VGet(pos_.x, pos_.y - ground_far, pos_.z));		// 実座標に戻す

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

#ifdef _DEBUG
	DrawFormatString(1100, 80, 0x000000, "towerHp = %d", hp_);
	DrawFormatString(20, 400, 0x000000, "towerPos = %f, %f, %f", pos_.x, pos_.y, pos_.z);
	DrawFormatString(20, 500, 0x000000, "checkPoint = %d", checkPoint_);
#endif
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
		VECTOR toTarget = VSub(checkPointPos, pos_);

		// 角度の取得
		angle_ = static_cast<float>(atan2(toTarget.x, toTarget.z));

		// 正規化
		toTarget = VNorm(toTarget);

		// 移動速度の反映
		vec_ = VScale(toTarget, to_goal_speed);
	}

	// 当たり判定を行い、その結果によって移動
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), isMove_, false, true, pos_, vec_, Collision::Chara::tower, colRadius_);

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	IsObstacleFront();

	// チェックポイントの当たり判定
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
