#pragma once
#include <DxLib.h>
#include <memory>

class Player;
class Camera;

class Shot
{
public:
	Shot(const char* fileName);
	virtual ~Shot();

	void Init();
	void Update();
	void Draw();

	// 弾が存在しているか
	bool isExist() const { return m_isExsit; }

	// 発射
	void Start(VECTOR pos, VECTOR vec);

	// ショットの中心位置取得
	VECTOR GetPos() const { return m_pos; };

	// 前のフレームの位置情報取得
	VECTOR GetLastPos() const { return m_lastPos; };

	// 半径の取得
	float GetRadius() const;

	// プレイヤーのポインタの取得
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// カメラのポインタの取得
	void SetCamera(std::shared_ptr<Camera> pCamera) { pCamera_ = pCamera; }

private:
	// ポインタ
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Camera> pCamera_;

	// 存在する
	bool m_isExsit = false;

	// 現在位置
	VECTOR m_pos;

	// 1フレーム前の位置
	VECTOR m_lastPos;

	// 移動方向
	VECTOR m_vec;

	int handle_ = -1;
};