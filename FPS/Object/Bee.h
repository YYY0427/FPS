#pragma once
#include "EnemyBase.h"

class Bee : public EnemyBase
{
public:
	Bee(const char* fileName);
	virtual ~Bee();

	void Update();

	// 2DのUIの表示
	void DrawUI();

	// ダメージを受けた
	void OnDamage(int damage);

	// 当たり判定の半径のゲッター
	float GetColRadius() const;

private:
	// プレイヤーが正面にいるかどうか
	bool IsPlayerFront() const;

	// プレイヤーに向かう
	void UpdateToPlayer();

	// 前に移動する(プレイヤーが見えていない)
	void UpdateToFront();

	// 方向転換を行う(プレイヤーが見えていない)
	void UpdateTurn();

	// 弾が当たったときのアニメーション
	void UpdateHitDamage();

	// 死亡アニメーション
	void UpdateDead();

private:
	// メンバー関数ポインタ
	void(Bee::* updateFunc_)();
};