#pragma once
#include <DxLib.h>
#include <memory>

class Player;
class Camera;
class Model;

class Shot
{
public:
	Shot();
	Shot(int handle);
	virtual ~Shot();

	void Update();
	void Draw();

	// モデルのロードのゲッター
	int LoadModel() const;

	// 弾が存在しているかのゲッター
	bool isExist() const { return isEnabled_; }

	// 発射
	void Start(VECTOR pos, VECTOR vec, std::shared_ptr<Player> pPlayer, std::shared_ptr<Camera> pCamera);

	// ショットの中心位置のゲッター
	VECTOR GetPos() const { return pos_; };

	// 前のフレームの位置情報のゲッター
	VECTOR GetLastPos() const { return lastPos_; };

	// 当たり判定の半径のゲッター
	float GetColRadius() const;

	// 弾を消すセッター
	void SetExsit(bool isExsit) { isEnabled_ = isExsit; }

private:
	// ポインタ
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Camera> pCamera_;
	std::shared_ptr<Model> pModel_;

	// 存在する
	bool isEnabled_;

	// モデルのハンドル
	int handle_;

	// 現在位置
	VECTOR pos_;

	// 1フレーム前の位置
	VECTOR lastPos_;

	// 移動方向
	VECTOR vec_;
};