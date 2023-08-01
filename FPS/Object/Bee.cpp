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
	// ファイルパス
	const char* const bee_adress = "Data/Model/beeCol.mv1";

	// 敵キャラクターの向いている方向
	constexpr VECTOR enemy_dir{ 0.0f, 0.0f, -1.0f };

	// 敵キャラクターの移動速度
	constexpr float to_front_speed = 4.0f;
	constexpr float to_player_speed = 4.0f;

	// 敵キャラクターの視野角
	constexpr float view_angle = 90.0f * DX_PI_F / 180.0f;

	// アニメーション番号
	constexpr int walk_anim_no = 1;
	constexpr int ondamage_anim_no = 2;
	constexpr int dead_anim_no = 0;
	constexpr int attack_anim_no = 3;

	// 当たり半径のサイズ
	constexpr float collision_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 50;

	// ダメージ受けた時の無敵時間
	constexpr int invincible_time = 10;

	// プレイヤーに攻撃する距離
	constexpr float player_attack_distance = 2000.0f;

	// タワーに攻撃する距離
	constexpr float tower_attack_distance = 2000.0f;

	// 目標を見失う距離
	constexpr float lost_distance = 4000.0f;

	// 攻撃の再使用まで待機フレーム数
	constexpr int attack_wait_time = 20;

	// 検知範囲
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

	// 敵から目標へのベクトルを求める
	toTargetVec_ = VSub(pPlayer_->GetPos(), pos_);

	// 角度の取得
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	if (isMove)
	{
		updateFunc_ = &Bee::UpdateToFront;
	}
	else
	{
		updateFunc_ = &Bee::UpdateToIdle;
	}

	// 3Dモデルの生成
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
	// ダメージ処理
	if (damageFrame_ > 0)	return;
	sHp_.hp_ -= damage;
	damageFrame_ = invincible_time;

	// HPがあればダメージアニメーションに移行
	// HPがなければ死亡アニメーションに移行
	if (sHp_.hp_ > 0)
	{
		// アニメーション設定
		animNo_ = ondamage_anim_no;
		pModel_->ChangeAnimation(animNo_, false, false, 4);

		// update変更
		updateFunc_ = &Bee::UpdateHitDamage;
	}
	else
	{
		// 死亡アニメーションに移行
		animNo_ = dead_anim_no;
		pModel_->ChangeAnimation(animNo_, false, false, 4);
		updateFunc_ = &Bee::UpdateDead;
	}
}

void Bee::Tracking(VECTOR pos, int target, float attackDistance)
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// 敵から目標へのベクトルを求める
	toTargetVec_ = VSub(pos, pos_);

	// Y軸は追いかけない
	toTargetVec_ = VGet(toTargetVec_.x, 0.0f, toTargetVec_.z);

	// 角度の取得
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	// 正規化
	toTargetVec_ = VNorm(toTargetVec_);

	// 移動速度の反映
	VECTOR vec = VScale(toTargetVec_, to_player_speed);

	// フィールドとの当たり判定を行い、その結果によって移動
	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), true, false, false, pos_, vec, Collision::Chara::bee, collision_radius);

	// ターゲットまでの距離
	float distans = VSize(VSub(pos, pos_));

	// 目標までまでの距離が特定距離以内なら攻撃アニメーションに移行
	if (distans < attackDistance)
	{
		// アニメーション設定
		animNo_ = attack_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		// 現在向かっている対象によって攻撃対象を決定
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

	// プレイヤーが死んでいる場合を追わない
	if (pPlayer_->GetIsDead())
	{
		updateFunc_ = &Bee::UpdateToFront;
		frameCount_ = 0;
	}

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// アニメーション更新処理
	pModel_->Update();

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	Sound();
}

void Bee::Attacking(VECTOR pos, int target, float attackDistance)
{
	assert(animNo_ == attack_anim_no);

	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// 敵から目標へのベクトルを求める
	toTargetVec_ = VSub(pos, pos_);

	// 角度の取得
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	// タワーの足元を狙うためY軸をプラス
	if(target == tower)
		toTargetVec_ = VGet(toTargetVec_.x, toTargetVec_.y + 500.0f, toTargetVec_.z);

	// プレイヤーまでの距離
	float distans = VSize(toTargetVec_);

	// 正規化
	toTargetVec_ = VNorm(toTargetVec_);

	// ショットのスピードをかける
	toTargetVec_ = VScale(toTargetVec_, 20.0f);

	// 
	if (attackWaitTimer_++ % attack_wait_time == 0)
	{
		pEnemyShotFactory_->ShootStart(pos_, toTargetVec_, target);
	}

	// プレイヤーから特定の距離離れていたらプレイヤーを追いかける
	if (attackDistance < distans)
	{
		// アニメーション設定
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		// updateを変更
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

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// アニメーション更新処理
	pModel_->Update();

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	Sound();
}

void Bee::UpdateToIdle()
{
	frameCount_++;
	if (!targetDiscover_ && frameCount_ > 120)
	{
		// タワーを見つけたらプレイヤーを追いかける
		if (IsTargetDetection(pTower_->GetPos(), pTower_->GetColRadius()) && !pPlayer_->GetIsDead())
		{
			target_ = tower;
		}
		// プレイヤーを見つけたらプレイヤーを追いかける
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
	// アニメーション更新処理
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
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// 現在敵が向いている方向のベクトルを生成する
	MATRIX enemyRotMtx = MGetRotY(angle_);

	// MATRIXをVECTORに変換
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// 移動速度を反映させる
	VECTOR vec = VScale(dir, to_front_speed);

	// フィールドとの当たり判定を行い、その結果によって移動
	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), true, false, false, pos_, vec, Collision::Chara::bee, collision_radius);

	frameCount_++;
	if (frameCount_ >= 2 * 60)
	{
		// タワーを見つけたらプレイヤーを追いかける
		if (IsTargetDetection(pTower_->GetPos(), pTower_->GetColRadius()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateTrackingToTower;
			frameCount_ = 0;
		}
		// プレイヤーを見つけたらプレイヤーを追いかける
		else if (IsTargetDetection(pPlayer_->GetPos(), pPlayer_->GetCollisionRadius()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateTrackingToPlayer;
			frameCount_ = 0;
		}

		// 見つからなかったら回転する
		else
		{
			// 回転する角度をランダムで計算
			rotSpeed_ = static_cast<float>(GetRand(250)) * 0.0001f;
			rotSpeed_ += 0.025f;
			if (GetRand(1)) rotSpeed_ *= -1.0f;
			
			// udpateを変更
			updateFunc_ = &Bee::UpdateTurn;
			frameCount_ = 0;
		}
	}

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// アニメーション更新処理
	pModel_->Update();

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ , 0.0f));

	Sound();
}

void Bee::UpdateTurn()
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// 回転の加算
	angle_ += rotSpeed_;

	frameCount_++;
	if (frameCount_ >= 30)
	{
		// タワーを見つけたらプレイヤーを追いかける
		if (IsPlayerFront(pTower_->GetPos()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateTrackingToTower;
			frameCount_ = 0;
		}
		// プレイヤーを見つけたらプレイヤーを追いかける
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

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// アニメーション更新処理
	pModel_->Update();
	
	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));

	Sound();
}

void Bee::UpdateHitDamage()
{
	assert(animNo_ == ondamage_anim_no);

	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// アニメーション更新処理
	pModel_->Update();

	// アニメーションが終わっていたら
	if (pModel_->IsAnimEnd())
	{
		// 待機アニメに変更する
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(walk_anim_no, true, false, 4);

		// Updateを待機に
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