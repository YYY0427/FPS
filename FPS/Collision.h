#pragma once
#include <DxLib.h>
#include <memory>

class Field;
class EnemyManager; 
class Player;
class Tower;

class Collision
{
public:
	Collision();
	virtual ~Collision();

	void Init();

	void CollCheck(int modelHandle, int objectModelHandle, VECTOR pos, VECTOR vec);

	void WallCollCheck(bool isMove, VECTOR vec);

	void FloorCollCheck(bool isJump);

	VECTOR ColisionToField(int modelHandle, bool isMove, bool isJump, VECTOR pos, VECTOR vec);

	VECTOR ColisionToTower(int modelHandle, bool isMove, bool isJump, VECTOR pos, VECTOR vec);

	float GetMaxY() const { return minY_; }

	void SetFieldManager(std::shared_ptr<Field> pField) { pField_ = pField; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }
	void SetTower(std::shared_ptr<Tower> pTower) { pTower_ = pTower; }

private:
	std::shared_ptr<Field> pField_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Tower> pTower_;

	MV1_COLL_RESULT_POLY* poly_{};
	HITRESULT_LINE lineRes_{};
	MV1_REF_POLYGONLIST refPoly_{};
	MV1_COLL_RESULT_POLY_DIM hitDim_{};

	// ポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* yuka_[2048]{};

	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* kabe_[2048]{};

	VECTOR moveAfterPos_;
	VECTOR oldPos_;

	float minY_;

	bool isHitFlag_;

	int yukaNum_;
	int kabeNum_;
};