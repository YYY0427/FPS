#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;
class Tower;
class Collision;
class EnemyShotFactory;

class EnemyBase
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	virtual void Update() = 0;
	void Draw();

	// 2DのUIの表示
	void DrawUI();

	// ダメージを受けた
	virtual void OnDamage(int damage) = 0;

	virtual int GetEnemyType() const = 0;

	bool GetIsAttak() const { return isAttack_; }

	// モデルのハンドルのゲッター
	int GetModelHandle() const;

	// 当たり判定のフレームインデックスのゲッター
	int GetColFrameIndex() const;

	// 位置のゲッター
	VECTOR GetPos() const { return pos_; }

	// 死亡情報のゲッター
	bool GetDead() const { return isDead_; }

	// 当たり判定の半径のゲッター
	float GetColRadius() const { return colRadius_; };

	enum EnemyType
	{
		enemy,
		bee,
	};

	enum Target
	{
		player,
		tower
	};

protected:
	// ターゲットが正面にいるかどうか
	bool IsPlayerFront(VECTOR targetPos) const;

	// プレイヤーに向かう
	virtual void UpdateTrackingToPlayer() = 0;

	// 前に移動する(プレイヤーが見えていない)
	virtual void UpdateToFront() = 0;

	// 方向転換を行う(プレイヤーが見えていない)
	virtual void UpdateTurn() = 0;

	// 弾が当たったときのアニメーション
	virtual void UpdateHitDamage() = 0;

	// 死亡アニメーション
	virtual void UpdateDead();

protected:
	// ポインタ
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<EnemyShotFactory> pEnemyShotFactory_;
	std::shared_ptr<Collision> pCollision_;

	// 現在再生しているアニメーション番号
	int animNo_ = 0;

	// アニメーション番号
	int deadAnimNo_ = 0;

	// フレームカウント
	int frameCount_ = 0;

	// 位置
	VECTOR pos_ = VGet(0, 0, 0);

	// プレイヤーからの距離
	VECTOR toTargetVec_ = VGet(0, 0, 0);

	// 向いている方向
	float angle_ = 0.0f;

	// 回転速度
	float rotSpeed_ = 0.0f;

	// HP
	int hp_ = 0;

	// 最大HP
	int maxHp_ = 0;

	// HPバーの横幅
	int hpBarWidth_ = 0;

	// HPバーの縦幅
	int hpBarHeight_ = 0;

	// 無敵時間
	int damageFrame_ = 0;

	// 死んでいるか
	bool isDead_ = false;

	// モデルハンドルのファイル
	const char* fileName_ = nullptr;

	// 当たり判定の半径
	float colRadius_ = 0.0f;

	// 視野角
	float viewAngle_ = 0.0f;

	// キャラクターの向いている方向
	VECTOR dir_ = VGet(0, 0, 0);

	// 敵が死んでから消えるまでの時間
	int deadDisappearTime_ = 0;

	// 攻撃の再使用まで待機フレームカウント
	int attackWaitTimer_ = 0;

	// 攻撃しているか
	bool isAttack_ = false;
};