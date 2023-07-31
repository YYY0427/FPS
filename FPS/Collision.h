#pragma once
#include <DxLib.h>
#include <memory>

class StageManager;
class EnemyManager; 
class Tower;
class ObstacleManager;
class Player;

/// <summary>
/// 当たり判定クラス 
/// </summary>
class Collision
{
public:
	Collision(StageManager* pStages, std::shared_ptr<Tower> pTower, std::shared_ptr<EnemyManager> pEnemyManager, std::shared_ptr<ObstacleManager> pObstacleManager, std::shared_ptr<Player> pPlayer);
	virtual ~Collision();

	// 押出当たり判定の初期化
	void ExtrusionCollisionInit();

	// ポリゴンと当たっているかチェックして壁ポリゴンか床ポリゴンか判定
	void CollsionCheckAndPolyTypeCheck(int modelHandle, int objectModelHandle, int collisionFrameIndex, VECTOR pos, VECTOR vec, float collisionRadius, int chara);

	// 壁ポリゴン当たり判定処理
	void WallPolyColCheckProcess(bool isMove, VECTOR vec);

	// 床ポリゴン当たり判定処理
	void FloorPolyColCheckProcess(bool isJump, bool isUseGravity, int chara);

	// ジャンプ中の場合の床ポリゴンとの当たり判定
	void JumpingFloorPolyColCheckProcess();

	// 押出当たり判定処理
	VECTOR ExtrusionColision(int modelHandle, bool isMove, bool isJump, bool isUseGravity, VECTOR pos, VECTOR vec, int chara, float collisionRadius);

	// ステージの地面の高さ
	float GetGroundY() const { return groundY_; }


	// 丸同士の当たり判定
	bool SpheresColision(VECTOR pos1, VECTOR pos2, float colisionRadius1, float colisionRadius2);

	/// <summary>
	/// モデルとカプセルの当たり判定
	/// </summary>
	/// <param name="modelHandle">モデルのハンドル</param>
	/// <param name="frameIndex">フレームのハンドル</param>
	/// <param name="pos">球の位置</param>
	/// <param name="collisionRadius">球の大きさ</param>
	/// <returns>当たったかどうか</returns>
	bool ModelAndCapsuleCollision(int modelHandle, int frameIndex, VECTOR pos, VECTOR lastPos, float collisionRadius);

	/// <summary>
	/// モデルと球の当たり判定
	/// </summary>
	/// <param name="modelHandle">モデルのハンドル</param>
	/// <param name="frameIndex">フレームのハンドル</param>
	/// <param name="pos">球の位置</param>
	/// <param name="collisionRadius">球の大きさ</param>
	/// <returns>当たったかどうか</returns>
	bool ModelAndSphereCollision(int modelHandle, int frameIndex, VECTOR pos, float collisionRadius);

	MV1_COLL_RESULT_POLY_DIM GetCollisionResult() const { return result_; }
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

	MV1_COLL_RESULT_POLY_DIM result_;

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