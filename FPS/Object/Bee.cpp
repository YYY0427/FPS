#include "Bee.h"
#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
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

	// 当たり半径のサイズ
	constexpr float col_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 50;

	// HPバーの表示
	constexpr int hp_bar_width = 64;
	constexpr int hp_bar_height = 10;

	// ダメージ受けた時の無敵時間
	constexpr int invincible_time = 60;
}

Bee::Bee(const char* fileName)
{
	updateFunc_ = &Bee::UpdateToFront;
	animNo_ = walk_anim_no;
	frameCount_ = 0;
	rotSpeed_ = 0;
	hp_ = max_hp;
	damageFrame_ = 0;
	isDead_ = false;
	fileName_ = fileName;

	Init();
}

Bee::~Bee()
{
}

void Bee::Update()
{
	(this->*updateFunc_)();
}

void Bee::DrawUI()
{
	// モデルのハンドル取得
	int handle = pModel_->GetModelHandle();

	// モデル内にあるHPバーを表示する座標のデータを取得する
	int frameNo = MV1SearchFrame(handle, "Head3_end");

	// HPバーを表示する座標データのワールド座標を取得する
	VECTOR hpPos = MV1GetFramePosition(handle, frameNo);

	hpPos.y += 30;

	// HPバー表示位置のワールド座標をスクリーン座標に変換する
	VECTOR screenPos = ConvWorldPosToScreenPos(hpPos);

	if ((screenPos.z <= 0.0f) || (screenPos.z >= 1.0f))
	{
		// 表示範囲外の場合何も表示しない
		return;
	}

	// 最大HPに対する現在のHPの割合を計算する
	float hpRate = static_cast<float>(hp_) / static_cast<float>(max_hp);

	// HPバーの長さを計算する
	float barWidth = hp_bar_width * hpRate;

	// HPバーの土台(赤)
//	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xff0000, true);

	// 現在のHP(緑)
	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x - hp_bar_width / 2 + barWidth, screenPos.y + hp_bar_height, 0x00ff00, true);

	// 枠線
	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xffffff, false);

#ifdef _DEBUG
//	DrawSphere3D(pos_, col_radius, 32, GetColor(255, 0, 0), GetColor(255, 255, 255), false);
#endif
}

float Bee::GetColRadius() const
{
	return col_radius;
}

void Bee::OnDamage(int damage)
{
	if (damageFrame_ > 0)	return;
	hp_ -= damage;
	damageFrame_ = invincible_time;

	if (hp_ > 0)
	{
		// 弾が当たった時のアニメーションに切り替える
		animNo_ = ondamage_anim;
		//	pModel_->ChangeAnimation(anim_hit_bullet, false, false, 60);
		pModel_->SetAnimation(animNo_, false, false);
		updateFunc_ = &Bee::UpdateHitDamage;
	}
	else
	{
		// 死亡アニメーションに移行
		animNo_ = dead_anim_no;
		pModel_->ChangeAnimation(dead_anim_no, false, false, 4);
		updateFunc_ = &Bee::UpdateDead;
	}
}

bool Bee::IsPlayerFront() const
{
	// 現在敵が向いている方向のベクトルを生成する
	MATRIX enemyRotMtx = MGetRotY(angle_);
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// 敵からプレイヤーへのベクトル
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), pos_);
	VECTOR toPlayerNorm = VNorm(toPlayer);

	// 内積から角度を求める
	float dot = VDot(dir, toPlayerNorm);
	float angle = acosf(dot);

	// 敵からプレイヤーへのベクトル
	float distans = VSize(toPlayer);

	// 視野の中にプレイヤーがいてプレイヤーまでの距離が指定の距離以内の場合
	// プレイヤーを追いかける
	if (angle < view_angle && distans < 1000.0f)
	{
		return true;
	}
	return false;
}

void Bee::UpdateToPlayer()
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// アニメーション更新処理
	pModel_->Update();
#if false
	// 現在敵が向いている方向のベクトルを生成する
	MATRIX enemyRotMtx = MGetRotY(m_angle);
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// 敵からプレイヤーへのベクトル
	VECTOR toPlayer = VSub(m_pPlayer->GetPos(), m_pos);
	toPlayer = VNorm(toPlayer);

	// 内積から角度を求める
	float dot = VDot(dir, toPlayer);
	float angle = acosf(dot);

	float rot = -0.05f;

	// 現在の進行方向、最終的に向かいたい方向の外積を回転軸にする
	VECTOR axis = VCross(toPlayer, dir);

	// 生成した回転軸を中心とした回転マトリクスを生成する
	MATRIX mtx = MGetRotAxis(axis, rot);

	// 現在の進行方向に回転マトリクスをかける
	dir = VTransform(dir, mtx);
	m_pos = VAdd(m_pos, VScale(dir, 10.0f));
#else
	// 敵からプレイヤーへのベクトルを求める
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), pos_);

	// 角度の取得
	angle_ = static_cast<float>(atan2(toPlayer.x, toPlayer.z));

	// 正規化
	toPlayer = VNorm(toPlayer);

	// 移動速度の反映
	VECTOR vec = VScale(toPlayer, to_player_speed);

	// 移動
	pos_ = VAdd(pos_, vec);
#endif

	// プレイヤーが死んでいる場合を追わない
	if (pPlayer_->GetIsDead())
	{
		updateFunc_ = &Bee::UpdateToFront;
		frameCount_ = 0;
	}

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, angle_ + DX_PI_F, 0.0f));
}

void Bee::UpdateToFront()
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// アニメーション更新処理
	pModel_->Update();

	// 現在敵が向いている方向のベクトルを生成する
	MATRIX enemyRotMtx = MGetRotY(angle_);

	// MATRIXをVECTORに変換
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// 移動速度を反映させる
	VECTOR vec = VScale(dir, to_front_speed);

	// 移動させる
	pos_ = VAdd(pos_, vec);

	frameCount_++;
	if (frameCount_ >= 2 * 60)
	{
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

	pModel_->SetPos(pos_);
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));
}

void Bee::UpdateTurn()
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// アニメーション更新処理
	pModel_->Update();

	angle_ += rotSpeed_;

	frameCount_++;
	if (frameCount_ >= 30)
	{
		if (IsPlayerFront() && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateToPlayer;
			frameCount_ = 0;
		}
		else if(!IsPlayerFront() && !pPlayer_->GetIsDead())
		{
			updateFunc_ = &Bee::UpdateToFront;
			frameCount_ = 0;
		}
	}

	pModel_->SetPos(pos_);
	pModel_->SetRot(VGet(0.0f, angle_, 0.0f));
}

void Bee::UpdateHitDamage()
{
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	assert(animNo_ == ondamage_anim);
	pModel_->Update();

	if (pModel_->IsAnimEnd())
	{
		// 待機アニメに変更する
		animNo_ = walk_anim_no;
		pModel_->ChangeAnimation(walk_anim_no, true, true, 4);

		// Updateを待機に
		updateFunc_ = &Bee::UpdateToPlayer;
	}
}

void Bee::UpdateDead()
{
	frameCount_++;
	assert(animNo_ == dead_anim_no);
	pModel_->Update();

	if (pModel_->IsAnimEnd() && frameCount_ > 120)
	{
		isDead_ = true;
		frameCount_ = 0;
	}
}