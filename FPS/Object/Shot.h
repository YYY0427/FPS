#pragma once
#include <DxLib.h>
#include <memory>

class Player;
class Camera;

class Shot
{
public:
	Shot();
	virtual ~Shot();

	void Init(int handle);
	void Update();
	void Draw();

	// モデルのロードのゲッター
	int LoadModel() const;

	// 弾が存在しているかのゲッター
	bool isExist() const { return isExsit_; }

	// 発射
	void Start(VECTOR pos, VECTOR vec);

	// ショットの中心位置のゲッター
	VECTOR GetPos() const { return pos_; };

	// 前のフレームの位置情報のゲッター
	VECTOR GetLastPos() const { return lastPos_; };

	// 弾を消すセッター
	void SetExsit(bool isExsit) { isExsit_ = isExsit; }

	// プレイヤーのポインタのセッター
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// カメラのポインタのセッター
	void SetCamera(std::shared_ptr<Camera> pCamera) { pCamera_ = pCamera; }
private:
	// ポインタ
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Camera> pCamera_;

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