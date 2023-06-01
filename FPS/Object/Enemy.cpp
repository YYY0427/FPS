#include "Enemy.h"
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
	constexpr int walk_anim = 8;
	constexpr int anim_hit_bullet = 4;
	constexpr int anim_dead = 3;

	// 当たり半径のサイズ
	constexpr float col_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 30;

	// HPバーの表示
	constexpr int hp_bar_width = 64;
	constexpr int hp_bar_height = 10;

	// ダメージ受けた時の無敵時間
	constexpr int invincible_time = 60;
}

Enemy::Enemy(const char* fileName, std::shared_ptr<Player> pPlayer) :
	pPlayer_(pPlayer),
	m_updateFunc(&Enemy::UpdateToFront),
	m_animNo(walk_anim),
	m_frameCount(0),
	m_rotSpeed(0),
	m_hp(max_hp),
	m_damageFrame(0),
	isDead_(false)
{
	// 3Dモデルの生成
	pModel_ = std::make_shared<Model>(fileName);
	pModel_->SetAnimation(m_animNo, true, true);
	pModel_->SetUseCollision(true, true);

	// 敵をランダムに配置
	m_pos.x = GetRand(2000) - 1000;
	m_pos.y = 0;
	m_pos.z = GetRand(2000) - 1000;
	m_angle = GetRand(360) * DX_PI_F / 180.0f;
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
}

void Enemy::Update()
{
	(this->*m_updateFunc)();
}

void Enemy::Draw()
{
	if (m_damageFrame > 0)
	{
		if (m_damageFrame % 2) return;
	}

	// モデルの描画
	pModel_->Draw();
}

void Enemy::DrawUI()
{
	// モデルのハンドル取得
	int handle = pModel_->GetModelHandle();

	// モデル内にあるHPバーを表示する座標のデータを取得する
	int frameNo = MV1SearchFrame(handle, "Head3_end");

	// HPバーを表示する座標データのワールド座標を取得する
	VECTOR hpPos = MV1GetFramePosition(handle, frameNo);

	// HPバー表示位置のワールド座標をスクリーン座標に変換する
	VECTOR screenPos = ConvWorldPosToScreenPos(hpPos);

	if ((screenPos.z <= 0.0f) || (screenPos.z >= 1.0f))
	{
		// 表示範囲外の場合何も表示しない
		return;
	}

	// 最大HPに対する現在のHPの割合を計算する
	float hpRate = static_cast<float>(m_hp) / static_cast<float>(max_hp);

	// HPバーの長さを計算する
	float barWidth = hp_bar_width * hpRate;

	// HPバーの土台(赤)
//	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xff0000, true);

	// 現在のHP(緑)
	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x - hp_bar_width / 2 + barWidth, screenPos.y + hp_bar_height, 0x00ff00, true);

	// 枠線
	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xffffff, false);
}

int Enemy::GetModelHandle() const
{
	return pModel_->GetModelHandle();
}

int Enemy::GetColFrameIndex() const
{
	return pModel_->GetColFrameIndex();
}

float Enemy::GetColRadius()
{
	return col_radius;
}

void Enemy::OnDamage(int damage)
{
	if (m_damageFrame > 0)	return;

	m_hp -= damage;
	m_damageFrame = invincible_time;

	if (m_hp > 0)
	{
		// 弾が当たった時のアニメーションに切り替える
		m_animNo = anim_hit_bullet;
		//	pModel_->ChangeAnimation(anim_hit_bullet, false, false, 60);
		pModel_->SetAnimation(m_animNo, false, false);
		m_updateFunc = &Enemy::UpdateHitBullet;
	}
	else
	{
		// 死亡アニメーションに移行
		m_animNo = anim_dead;
		pModel_->ChangeAnimation(anim_dead, false, false, 4);
		m_updateFunc = &Enemy::UpdateDead;
	}
}

bool Enemy::IsPlayerFront() const
{
	// 現在敵が向いている方向のベクトルを生成する
	MATRIX enemyRotMtx = MGetRotY(m_angle);
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// 敵からプレイヤーへのベクトル
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), m_pos);
	toPlayer = VNorm(toPlayer);

	// 内積から角度を求める
	float dot = VDot(dir, toPlayer);
	float angle = acosf(dot);

	return (angle < view_angle);
}

void Enemy::UpdateToPlayer()
{
	m_damageFrame--;
	if (m_damageFrame < 0) m_damageFrame = 0;

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
	// 敵からプレイヤーへのベクトル(正規化)
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), m_pos);
	toPlayer = VNorm(toPlayer);

	m_pos = VAdd(m_pos, VScale(toPlayer, to_player_speed));
#endif
	pModel_->SetPos(m_pos);
	pModel_->SetRot(VGet(0.0f, m_angle, 0.0f));
}

void Enemy::UpdateToFront()
{
	m_damageFrame--;
	if (m_damageFrame < 0) m_damageFrame = 0;

	pModel_->Update();

	// 現在敵が向いている方向のベクトルを生成する
	MATRIX enemyRotMtx = MGetRotY(m_angle);
	VECTOR dir = VTransform(enemy_dir, enemyRotMtx);

	// 移動速度を反映させる
	VECTOR vec = VScale(dir, to_front_speed);

	// 移動させる
	m_pos = VAdd(m_pos, vec);

	m_frameCount++;
	if (m_frameCount >= 2 * 60)
	{
		m_rotSpeed = static_cast<float>(GetRand(250)) * 0.0001f;
		m_rotSpeed += 0.025;
		if (GetRand(1)) m_rotSpeed *= -1.0f;

		m_updateFunc = &Enemy::UpdateTurn;
		m_frameCount = 0;
	}

	pModel_->SetPos(m_pos);
	pModel_->SetRot(VGet(0.0f, m_angle, 0.0f));
}

void Enemy::UpdateTurn()
{
	m_damageFrame--;
	if (m_damageFrame < 0) m_damageFrame = 0;

	pModel_->Update();

	m_angle += m_rotSpeed;
	m_frameCount++;
	if (m_frameCount >= 30)
	{
		if (IsPlayerFront())
		{
			m_updateFunc = &Enemy::UpdateToPlayer;
			m_frameCount = 0;
		}
		else
		{
			m_updateFunc = &Enemy::UpdateToFront;
			m_frameCount = 0;
		}
	}

	pModel_->SetPos(m_pos);
	pModel_->SetRot(VGet(0.0f, m_angle, 0.0f));
}

void Enemy::UpdateHitBullet()
{
	m_damageFrame--;
	if (m_damageFrame < 0) m_damageFrame = 0;

	assert(m_animNo == anim_hit_bullet);
	pModel_->Update();

	if (pModel_->IsAnimEnd())
	{
		// 待機アニメに変更する
		m_animNo = walk_anim;
		pModel_->ChangeAnimation(walk_anim, true, true, 4);

		// Updateを待機に
		m_updateFunc = &Enemy::UpdateToPlayer;
	}
}

void Enemy::UpdateDead()
{
	m_frameCount++;
	assert(m_animNo == anim_dead);
	pModel_->Update();

	if (pModel_->IsAnimEnd() && m_frameCount > 120)
	{
		isDead_ = true;
		m_frameCount = 0;
	}
}
