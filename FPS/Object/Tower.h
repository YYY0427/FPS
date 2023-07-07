#pragma once
#include <memory>
#include <DxLib.h>
#include "../Model.h"

class Model;
class Collision;
class EnemyManager;

class Tower
{
public:
	Tower();
	virtual ~Tower();

	void Init();
	void Update();
	void Draw();

	void OnDamage(int damage);

	// 目的地に向かう処理
	void HeadToDestination(VECTOR checkPoint);

	// 敵がフィールドに存在するか
	bool IsEnemyExists();

	// 現在どのチェックポイントを目指すか設定
	void CheckPointSet();

	// セッター
	void SetCollision(Collision* pCollision) { pCollision_ = pCollision; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }

	// ゲッター
	int GetModelHandle() const { return pModel_->GetModelHandle(); }
	VECTOR GetPos() const { return pos_; }
	float GetColRadius() const { return colRadius_; }
	int GetIsDead() const { return isDead_; }
	bool GetIsGoal() const { return isGoal_; }

private:	
	enum
	{
		k_check_point1 = 1,
		k_check_point2,
		k_check_point3,
		k_check_point4,
		k_check_point5,
	};

private:
	// メンバー関数ポインタ
//	void(Tower::* updateFunc_)();

	// ポインタ
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	Collision* pCollision_;

	// 位置
	VECTOR pos_;

	// 移動ベクトル
	VECTOR vec_;

	// 現在目指しているチェックポイント
	VECTOR checkPointNow_;

	// 
	int checkPoint_;

	// ゴールしたか
	bool isGoal_;

	// 現在のHP
	int hp_;

	// ダメージを受けてから何フレーム経ったか
	int damageFrame_;

	// 当たり判定の半径
	float colRadius_;

	// 死んだか
	bool isDead_;

	// 角度(向いている方向)
	float angle_;

	// 動いているか
	bool isMove_;
};