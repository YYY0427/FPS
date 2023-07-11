#pragma once
#include <DxLib.h>
#include <memory>

class Model;
class Tower;

class EnemyShot
{
public:
	EnemyShot();
	virtual ~EnemyShot();

	void Init(int handle);
	void Update();
	void Draw();

	// モデルのロードのゲッター
	int LoadModel() const;

	void Start(VECTOR pos, VECTOR vec);

	// 当たり判定の半径のゲッター
	float GetColRadius() const;

	// 弾を消すセッター
	void SetExsit(bool isExsit) { isExsit_ = isExsit; }

	// プレイヤーのポインタのセッター
	void SetTower(std::shared_ptr<Tower> pTower) { pTower_ = pTower; }
private:
	// ポインタ
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Tower> pTower_;

	// 存在する
	bool isExsit_;

	// モデルのハンドル
	int handle_;

	// 現在位置
	VECTOR pos_;

	// 1フレーム前の位置
	VECTOR lastPos_;

	// 移動方向
	VECTOR vec_;
};