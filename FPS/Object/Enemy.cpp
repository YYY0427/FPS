#include "Enemy.h"
#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
#include "Tower.h"
#include "../Collision.h"
#include <cassert>

namespace
{
	// ファイルパス
	const char* const enemy_adress = "Data/Model/enemyCol.mv1";

	// 敵キャラクターの向いている方向
	constexpr VECTOR enemy_dir{ 0.0f, 0.0f, -1.0f };

	// 敵キャラクターの移動速度
	constexpr float to_front_speed = 8.0f;
	constexpr float to_player_speed = 8.0f;

	// 敵キャラクターの視野角
	constexpr float view_angle = 30.0f * DX_PI_F / 180.0f;

	// アニメーション番号
	constexpr int walk_anim_no = 8;
	constexpr int ondamage_anim_no = 4;
	constexpr int dead_anim_no = 3;
	constexpr int attack_anim_no = 0;
	constexpr int idle_anim_no = 2;

	// 当たり半径のサイズ
	constexpr float collision_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 30;

	// ダメージ受けた時の無敵時間
	constexpr int invincible_time = 10;

	// プレイヤーに攻撃する距離
	constexpr float player_attack_distance = 140.0f;

	// タワーに攻撃する距離
	constexpr float tower_attack_distance = 250.0f;

	// 目標を見失う距離
	constexpr float lost_distance = 2000.0f;

	// 攻撃の再使用まで待機フレーム数
	constexpr int attack_wait_time = 20;

	// 検知範囲
	constexpr float detection_range = 1500.0f;
}

Enemy::Enemy(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, VECTOR pos, bool isMove)
{
	pPlayer_ = pPlayer;
	pTower_ = pTower;
	pCollision_ = pCollision;
	pEnemyShotFactory_ = pEnemyShotFactory;

	pos_ = pos;
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
	detectionRange_ = detection_range;

	// 敵から目標へのベクトルを求める
	toTargetVec_ = VSub(pPlayer_->GetPos(), pos_);

	// 角度の取得
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	if (isMove)
	{
		animNo_ = walk_anim_no;
		updateFunc_ = &Enemy::UpdateToFront;
	}
	else
	{
		animNo_ = idle_anim_no;
		updateFunc_ = &Enemy::UpdateToIdle;	
	}

	// 3Dモデルの生成
	pModel_ = std::make_shared<Model>(enemy_adress);
	pModel_->SetAnimation(animNo_, true, true);
	pModel_->SetUseCollision(true);
	pModel_->SetPos(pos);
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));
	pModel_->Update();
}

Enemy::~Enemy()
{

}

void Enemy::Update()
{
	(this->*updateFunc_)();
}

void Enemy::OnDamage(int damage)
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
		updateFunc_ = &Enemy::UpdateHitDamage;
	}
	else
	{
		// 死亡アニメーションに移行
		animNo_ = dead_anim_no;
		pModel_->ChangeAnimation(animNo_, false, false, 4);
		updateFunc_ = &Enemy::UpdateDead;
	}
}

void Enemy::Tracking(VECTOR pos, int target, float attackDistance)
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// 敵から目標へのベクトルを求める
	toTargetVec_ = VSub(pos, pos_);

	// 角度の取得
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	// 正規化
	toTargetVec_ = VNorm(toTargetVec_);

	// 移動速度の反映
	VECTOR vec = VScale(toTargetVec_, to_player_speed);

	
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
			updateFunc_ = &Enemy::UpdateAttackToPlayer;
			break;
		case tower:
			updateFunc_ = &Enemy::UpdateAttackToTower;
			break;
		}
		frameCount_ = 0;
	}
	if (target == player && distans > lost_distance)
	{
		updateFunc_ = &Enemy::UpdateTrackingToTower;
	}

	// プレイヤーが死んでいる場合を追わない
	if (pPlayer_->GetIsDead())
	{
		updateFunc_ = &Enemy::UpdateToFront;
		frameCount_ = 0;
	}

	// 当たり判定を行い、その結果によって移動
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), true, false, true, pos_, vec, Collision::Chara::enemy, collision_radius);

	// 位置座標の設定
	pModel_->SetPos(pos_);

	MV1RefreshCollInfo(pModel_->GetModelHandle(), -1);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	// アニメーション更新処理
	pModel_->Update();
}

void Enemy::Attacking(VECTOR pos, int target, float attacDistance)
{
	assert(animNo_ == attack_anim_no);

	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// 指定されたフレームに1回攻撃する
	if (attackWaitTimer_++ % attack_wait_time == 0)
	{
		// 攻撃
		isAttack_ = true;
	}
	else
	{
		isAttack_ = false;
	}

	// 敵から目標へのベクトルを求める
	toTargetVec_ = VSub(pos, pos_);

	// 角度の取得
	angle_ = static_cast<float>(atan2(toTargetVec_.x, toTargetVec_.z));

	// ターゲットまでの距離
	float distans = VSize(toTargetVec_);

	// ターゲットから特定の距離離れていたらプレイヤーを追いかける
	if (attacDistance < distans)
	{
		// アニメーション設定
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		// updateを変更
		switch (target)
		{
		case player:
			updateFunc_ = &Enemy::UpdateTrackingToPlayer;
			break;
		case tower:
			updateFunc_ = &Enemy::UpdateTrackingToTower;
		}
		frameCount_ = 0;
	}

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));

	// アニメーション更新処理
	pModel_->Update();
}

void Enemy::UpdateToIdle()
{
	// タワーを見つけたらプレイヤーを追いかける
	if (IsTargetDetection(pTower_->GetPos(), pTower_->GetColRadius()) && !pPlayer_->GetIsDead())
	{
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		updateFunc_ = &Enemy::UpdateTrackingToTower;
		frameCount_ = 0;
	}
	// プレイヤーを見つけたらプレイヤーを追いかける
	else if (IsTargetDetection(pPlayer_->GetPos(), pPlayer_->GetColRadius()) && !pPlayer_->GetIsDead())
	{
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		updateFunc_ = &Enemy::UpdateTrackingToPlayer;
		frameCount_ = 0;
	}
	
	// フィールドとの当たり判定を行い、その結果によって移動
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), false , false, true, pos_, VGet(0.0f, 0.0f, 0.0f), Collision::Chara::enemy, collision_radius);

	pModel_->SetPos(pos_);

	// アニメーション更新処理
	pModel_->Update();
}

void Enemy::UpdateTrackingToPlayer()
{
	Tracking(pPlayer_->GetPos(), player, player_attack_distance);
}

void Enemy::UpdateTrackingToTower()
{
	Tracking(pTower_->GetPos(), tower, tower_attack_distance);
}

void Enemy::UpdateToFront()
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
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), true, false, true, pos_, vec, Collision::Chara::enemy, collision_radius);

	frameCount_++;
	if (frameCount_ >= 2 * 60)
	{
		// タワーを見つけたらプレイヤーを追いかける
		if (IsTargetDetection(pTower_->GetPos(), pTower_->GetColRadius()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Enemy::UpdateTrackingToTower;
			frameCount_ = 0;
		}
		// プレイヤーを見つけたらプレイヤーを追いかける
		else if (IsTargetDetection(pPlayer_->GetPos(), pPlayer_->GetColRadius()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Enemy::UpdateTrackingToPlayer;
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
			updateFunc_ = &Enemy::UpdateTurn;
			frameCount_ = 0;
		}	
	}

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));

	// アニメーション更新処理
	pModel_->Update();
}

void Enemy::UpdateAttackToPlayer()
{
	Attacking(pPlayer_->GetPos(), player, player_attack_distance);
}

void Enemy::UpdateAttackToTower()
{
	Attacking(pTower_->GetPos(), tower, tower_attack_distance);
}

void Enemy::UpdateTurn()
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// 回転の加算
	angle_ += rotSpeed_;

	frameCount_++;
	if (frameCount_ >= 30)
	{
		if (IsPlayerFront(pPlayer_->GetPos()) && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Enemy::UpdateTrackingToPlayer;
			frameCount_ = 0;
		}
		else
		{
			updateFunc_ = &Enemy::UpdateToFront;
			frameCount_ = 0;
		}
	}

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// アニメーション更新処理
	pModel_->Update();

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ , 0.0f));
}

void Enemy::UpdateHitDamage()
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
		updateFunc_ = &Enemy::UpdateTrackingToPlayer;
	}
}