#pragma once
#include "EnemyBase.h"
class EnemyBoss : public EnemyBase
{
public:
	EnemyBoss(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, MainScene* pMainScene, VECTOR pos, bool isMove, int handle);
	virtual ~EnemyBoss();

	void Update();

	// ダメージを受けた
	void OnDamage(int damage);

	int GetEnemyType() const { return enemyBos; }

private:
	// 追跡処理
	void Tracking(VECTOR pos, int target, float attackDistance);

	// 攻撃処理
	void Attacking(VECTOR pos, int target, float attacDistance);

	// 待機
	void UpdateToIdle();

	// プレイヤーを追跡
	void UpdateTrackingToPlayer();

	// タワーを追跡
	void UpdateTrackingToTower();

	// プレイヤーに攻撃する
	void UpdateAttackToPlayer();

	// タワーに攻撃する
	void UpdateAttackToTower();

	// 前に移動する(プレイヤーが見えていない)
	void UpdateToFront();

	// 方向転換を行う(プレイヤーが見えていない)
	void UpdateTurn();

	// 弾が当たったときのアニメーション
	void UpdateHitDamage();

	void UpdateToGameClear();

	// 敵が攻撃してからもう一度攻撃するまでの待機
	void UpdateAttackWaitTimeToPlayer();
	void UpdateAttackWaitTimeToTower();
	void WaitTime(int target, VECTOR pos, float attacDistance);

private:
	// メンバー関数ポインタ
	void(EnemyBoss::* updateFunc_)();

	bool isPass_ = false;
};