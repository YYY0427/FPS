#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;
class Tower;
class Collision;
class EnemyShotFactory;
class MainScene;

class EnemyBase
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	virtual void Update() = 0;
	void Draw();

	// ダメージを受けた
	virtual void OnDamage(int damage) = 0;

	virtual int GetEnemyType() const = 0;

	bool GetIsAttak() const { return isAttack_; }

	void SetIsDead(bool isDead) { isDead_ = isDead; }

	// モデルのハンドルのゲッター
	int GetModelHandle() const;

	// 当たり判定のフレームインデックスのゲッター
	int GetColFrameIndex() const;

	// 位置のゲッター
	VECTOR GetPos() const { return pos_; }

	// 死亡情報のゲッター
	bool GetDead() const { return isDead_; }

	// 当たり判定の半径のゲッター
	float GetCollisionRadius() const { return colRadius_; };

	enum EnemyType
	{
		enemy,
		enemyBos,
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

	// ターゲットが検知範囲に入ったかどうか
	bool IsTargetDetection(VECTOR targetPos, float targetCollisionRadius) const;

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
	struct HP
	{
		// HP
		int hp_ = 0;

		// 最大HP
		int maxHp_ = 0;

		// HPを表示する際指定位置からどれだけY軸をプラスするか
		float hpUIDrawY_ = 0.0f;
	};

public:
	HP GetHP() const { return sHp_; }

protected:
	// ポインタ
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<EnemyShotFactory> pEnemyShotFactory_;
	std::shared_ptr<Collision> pCollision_;
	MainScene* pMainScene_;

	HP sHp_;

	// 現在再生しているアニメーション番号
	int animNo_ = 0;

	// アニメーション番号
	int deadAnimNo_ = 0;

	int discoverAnimNo_ = 0;

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

	// 検知範囲
	float detectionRange_ = 0.0f;

	int handle_ = -1;

	bool targetDiscover_ = false;

	int target_ = 0;
};