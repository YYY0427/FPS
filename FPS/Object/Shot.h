#pragma once
#include <DxLib.h>
#include <memory>

class Player;

class Shot
{
public:
	Shot();
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

private:
	// プレイヤーのポインタ
	std::shared_ptr<Player> pPlayer_;

	// 存在する
	bool m_isExsit = false;

	// 現在位置
	VECTOR m_pos;

	// 1フレーム前の位置
	VECTOR m_lastPos;

	// 移動方向
	VECTOR m_vec;
};