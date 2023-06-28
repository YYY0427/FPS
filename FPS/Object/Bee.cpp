#include "Bee.h"
#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
#include "../Collision.h"
#include "Tower.h"
#include <cassert>

namespace
{
	// 敵キャラクターの向いている方向
	constexpr VECTOR enemy_dir{ 0.0f, 0.0f, -1.0f };

	// 敵キャラクターの移動速度
	constexpr float to_front_speed = 4.0f;
	constexpr float to_player_speed = 4.0f;

	// 敵キャラクターの視野角
	constexpr float view_angle = 30.0f * DX_PI_F / 180.0f;

	// アニメーション番号
	constexpr int walk_anim_no = 1;
	constexpr int ondamage_anim = 2;
	constexpr int dead_anim_no = 0;
	constexpr int attack_anim_no = 3;

	// 当たり半径のサイズ
	constexpr float col_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 50;

	// HPバーの表示
	constexpr int hp_bar_width = 64;
	constexpr int hp_bar_height = 10;

	// ダメージ受けた時の無敵時間
	constexpr int invincible_time = 60;

	// プレイヤーに攻撃する距離
	constexpr float attack_distance = 140.0f;
}

Bee::Bee(const char* fileName)
{
	updateFunc_ = &Bee::UpdateToTower;
	animNo_ = walk_anim_no;
	frameCount_ = 0;
	rotSpeed_ = 0;
	hp_ = max_hp;
	damageFrame_ = 0;
	isDead_ = false;
	fileName_ = fileName;
	colRadius_ = col_radius;
	hpBarHeight_ = hp_bar_height;
	hpBarWidth_ = hp_bar_width;
	maxHp_ = max_hp;
	viewAngle_ = view_angle;
	dir_ = enemy_dir;
	deadDisappearTime_ = 120;
	deadAnimNo_ = dead_anim_no;

	Init();
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
	hp_ -= damage;
	damageFrame_ = invincible_time;

	// HPがあればダメージアニメーションに移行
	// HPがなければ死亡アニメーションに移行
	if (hp_ > 0)
	{
		// アニメーション設定
		animNo_ = ondamage_anim;
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

void Bee::Tracking(VECTOR pos, int target)
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// 敵から目標へのベクトルを求める
	toTarget_ = VSub(pos, pos_);

	// 角度の取得
	angle_ = static_cast<float>(atan2(toTarget_.x, toTarget_.z));

	// 正規化
	toTarget_ = VNorm(toTarget_);

	// 移動速度の反映
	VECTOR vec = VScale(toTarget_, to_player_speed);

	// フィールドとの当たり判定を行い、その結果によって移動
	pos_ = pCollision_->ColisionToField(pModel_->GetModelHandle(), true, false, pos_, vec);

	// ターゲットまでの距離
	float distans = VSize(VSub(pos, pos_));

	// 目標までまでの距離が特定距離以内なら攻撃アニメーションに移行
	if (distans < attack_distance)
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

	// プレイヤーが死んでいる場合を追わない
	if (pPlayer_->GetIsDead())
	{
		updateFunc_ = &Bee::UpdateToFront;
		frameCount_ = 0;
	}

	// アニメーション更新処理
	pModel_->Update();

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));
}

void Bee::Attacking(VECTOR pos, int target)
{
	assert(animNo_ == attack_anim_no);

	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// プレイヤーまでの距離
	float distans = VSize(VSub(pos, pos_));

	// プレイヤーから特定の距離離れていたらプレイヤーを追いかける
	if (attack_distance < distans)
	{
		// アニメーション設定
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(animNo_, true, false, 4);

		// updateを変更
		switch (target)
		{
		case player:
			updateFunc_ = &Bee::UpdateToPlayer;
			break;
		case tower:
			updateFunc_ = &Bee::UpdateToTower;
		}
		frameCount_ = 0;
	}

	// アニメーション更新処理
	pModel_->Update();

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));
}

void Bee::UpdateToPlayer()
{
	Tracking(pPlayer_->GetPos(), player);
}

void Bee::UpdateToTower()
{
	Tracking(pTower_->GetPos(), tower);
}

void Bee::UpdateAttackToPlayer()
{
	Attacking(pPlayer_->GetPos(), player);
}

void Bee::UpdateAttackToTower()
{
	Attacking(pTower_->GetPos(), tower);
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
	pos_ = pCollision_->ColisionToField(pModel_->GetModelHandle(), true, false, pos_, vec);

	frameCount_++;
	if (frameCount_ >= 2 * 60)
	{
		// プレイヤーを見つけたらプレイヤーを追いかける
		// 見つからなかったら回転する
		if (IsPlayerFront() && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateToPlayer;
			frameCount_ = 0;
		}
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

	// アニメーション更新処理
	pModel_->Update();

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));
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
		if (IsPlayerFront() && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateToPlayer;
			frameCount_ = 0;
		}
		else
		{
			updateFunc_ = &Bee::UpdateToFront;
			frameCount_ = 0;
		}
	}

	// アニメーション更新処理
	pModel_->Update();

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));
}

void Bee::UpdateHitDamage()
{
	assert(animNo_ == ondamage_anim);

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
		updateFunc_ = &Bee::UpdateToPlayer;
	}
}