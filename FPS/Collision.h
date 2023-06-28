#pragma once
#include <DxLib.h>
#include <memory>

class FieldManager;
class EnemyManager;
class Player;

class Collision
{
public:
	Collision();
	virtual ~Collision();

	void Init();
	VECTOR ColisionToField(int modelHandle, bool isMove, bool isJump, VECTOR pos, VECTOR vec);

	float GetMaxY() const { return maxY_; }

	void SetFieldManager(std::shared_ptr<FieldManager> pFieldManager) { pFieldManager_ = pFieldManager; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

private:
	std::shared_ptr<FieldManager> pFieldManager_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Player> pPlayer_;

	// ポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* yuka_[2048]{};

	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* kabe_[2048]{};

	VECTOR moveAfterPos_;

	float maxY_;
};