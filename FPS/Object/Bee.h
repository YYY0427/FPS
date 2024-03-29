#pragma once
#include "EnemyBase.h"

class Bee : public EnemyBase
{
public:
	Bee(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, MainScene* pMainScene, VECTOR pos, bool isMove, int handle);
	virtual ~Bee();

	void Update();

	// ダメージを受けた
	void OnDamage(int damage);
	
	int GetEnemyType() const { return bee; };

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

	void Sound();
private:
	// メンバー関数ポインタ
	void(Bee::* updateFunc_)();

	float degree_;
};