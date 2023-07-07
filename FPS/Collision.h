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

	// 初期化
	void Init();

	// ポリゴンと当たっているかチェックして壁ポリゴンか床ポリゴンか判定
	void CollCheck(int modelHandle, int objectModelHandle, VECTOR pos, VECTOR vec);

	// 壁ポリゴン当たり判定処理
	void WallPolyColCheckProcess(bool isMove, VECTOR vec);

	// 床ポリゴン当たり判定処理
	void FloorPolyColCheckProcess(bool isJump, int chara);

	// 当たり判定処理
	VECTOR Colision(int modelHandle, bool isMove, bool isJump, VECTOR pos, VECTOR vec, int chara);

	// ゲッター
	float GetPlayerMinY() const { return playerMinY_; }

	// セッター
	void SetFieldManager(std::shared_ptr<Field> pField) { pField_ = pField; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }
	void SetTower(std::shared_ptr<Tower> pTower) { pTower_ = pTower; }

public:
	enum Chara
	{
		player,
		enemy,
		tower
	};

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

	// 移動後の座標
	VECTOR moveAfterPos_;

	// 移動前の座標
	VECTOR oldPos_;

	// ポリゴンと当たったか
	bool isHitFlag_;

	// ポリゴン数
	int yukaNum_;
	int kabeNum_;

	// 床ポリゴンの高さ
	float playerMinY_;
};