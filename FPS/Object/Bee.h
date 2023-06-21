#pragma once
#include "EnemyBase.h"

class Bee : public EnemyBase
{
public:
	Bee(const char* fileName);
	virtual ~Bee();

	void Update();

	// ダメージを受けた
	void OnDamage(int damage);

private:
	// プレイヤーに向かう
	void UpdateToPlayer();

	// 前に移動する(プレイヤーが見えていない)
	void UpdateToFront();

	// 攻撃する
	void UpdateToAttack();

	// 方向転換を行う(プレイヤーが見えていない)
	void UpdateTurn();

	// 弾が当たったときのアニメーション
	void UpdateHitDamage();
private:
	// メンバー関数ポインタ
	void(Bee::* updateFunc_)();
};