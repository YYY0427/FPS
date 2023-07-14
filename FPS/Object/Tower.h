#pragma once
#include <memory>
#include <DxLib.h>
#include "../Model.h"

class Model;
class Collision;
class EnemyManager;
class StageManager;

class Tower
{
public:
	Tower(StageManager* pStageManager);
	virtual ~Tower();

	void Update();
	void Draw();

	void OnDamage(int damage);

	// 目的地に向かう処理
	void HeadToDestination(VECTOR checkPoint);

	// 敵がフィールドに存在するか
	bool IsEnemyEnabled();

	// 現在どのチェックポイントを目指すか設定
	void CheckPointSet();

	// セッター
	void SetCollision(std::shared_ptr<Collision> pCollision) { pCollision_ = pCollision; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }
	
	// ゲッター
	int GetModelHandle() const { return pModel_->GetModelHandle(); }
	VECTOR GetPos() const { return pos_; }
	float GetColRadius() const { return colRadius_; }
	int GetIsDead() const { return isDead_; }
	bool GetIsGoal() const { return isGoal_; }
	int GetCheckPoint() const { return checkPoint_; }

public:	
	enum CheckPoint
	{
		start,
		check_point1,
		check_point2,
		check_point3,
		check_point4,
		goal
	};

private:
	// メンバー関数ポインタ
//	void(Tower::* updateFunc_)();

	// ポインタ
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Collision> pCollision_;
	StageManager* pStageManager_;

	// 位置
	VECTOR pos_;

	// 移動ベクトル
	VECTOR vec_;

	// それぞれのチェックポイントの位置
	VECTOR checkPointPos1_;
	VECTOR checkPointPos2_;
	VECTOR checkPointPos3_;
	VECTOR checkPointPos4_;

	// 現在目指しているチェックポイントの位置
	VECTOR checkPointPos_;
	
	// 現在目指しているチェックポイントをint型にしたもの
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