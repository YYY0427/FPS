#pragma once
#include <DxLib.h>
#include <memory>

class StageManager;
class EnemyManager; 
class Tower;
class ObstacleManager;
class Player;

class Collision
{
public:
	Collision(StageManager* pStages, std::shared_ptr<Tower> pTower, std::shared_ptr<EnemyManager> pEnemyManager, std::shared_ptr<ObstacleManager> pObstacleManager, std::shared_ptr<Player> pPlayer);
	virtual ~Collision();

	// 初期化
	void Init();

	// ポリゴンと当たっているかチェックして壁ポリゴンか床ポリゴンか判定
	void CollCheck(int modelHandle, int objectModelHandle, int collisionFrameIndex, VECTOR pos, VECTOR vec, float collisionRadius, int chara);

	// 壁ポリゴン当たり判定処理
	void WallPolyColCheckProcess(bool isMove, VECTOR vec);

	// 床ポリゴン当たり判定処理
	void FloorPolyColCheckProcess(bool isJump, bool isUseGravity, int chara);

	// ジャンプ中の場合の床ポリゴンとの当たり判定
	void JumpingFloorPolyColCheckProcess();

	// 当たり判定処理
	VECTOR MovingColision(int modelHandle, bool isMove, bool isJump, bool isUseGravity, VECTOR pos, VECTOR vec, int chara, float collisionRadius);

//	void HitCollision();

	float GetGroundY() const { return groundY_; }

public:
	enum Chara
	{
		player,
		enemy,
		bee,
		tower
	};

private:
	StageManager* pStages_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<ObstacleManager> pObstacleManager_;
	std::shared_ptr<Player> pPlayer_;

	MV1_COLL_RESULT_POLY* poly_{};
	HITRESULT_LINE lineRes_{};
	MV1_REF_POLYGONLIST refPoly_{};
	MV1_COLL_RESULT_POLY_DIM hitDim_{};

	// ポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* yuka_[2048]{};

	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* kabe_[2048]{};

	// 移動後の座標
	VECTOR moveAfterPos_;

	// 移動前の座標
	VECTOR oldPos_;

	// ポリゴンと当たったか
	bool isHitFlag_;

	// ポリゴン数
	int yukaNum_;
	int kabeNum_;

	float groundY_;
};