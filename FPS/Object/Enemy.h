#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;

class Enemy
{
public:
	Enemy(const char* fileName, std::shared_ptr<Player> pPlayer);
	virtual ~Enemy();

	void Init();
	void Update();
	void Draw();

	// 2DのUIの表示
	void DrawUI();	

	// モデルのハンドル取得
	int GetModelHandle() const;

	// 当たり判定のフレームインデックス取得
	int GetColFrameIndex() const;

	// 位置取得
	VECTOR GetPos() const { return pos_; }

	// 死亡情報の取得
	bool GetDead() const { return isDead_; }

	// 当たり判定の半径
	float GetColRadius();

	// ダメージを受けた
	void OnDamage(int damage);

private:
	// プレイヤーが正面にいるかどうか
	bool IsPlayerFront() const;

	// プレイヤーに向かう
	void UpdateToPlayer();

	// 前に移動する(プレイヤーが見えていない)
	void UpdateToFront();

	// 方向転換を行う(プレイヤーが見えていない)
	void UpdateTurn();

	// 弾が当たったときのアニメーション
	void UpdateHitDamage();

	// 死亡アニメーション
	void UpdateDead();

private:
	// ポインタ
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Model> pModel_;

	// メンバー関数ポインタ
	void(Enemy::* m_updateFunc)();

	// 再生しているアニメーション番号
	int animNo_;

	// フレームカウント
	int frameCount_;

	// 位置
	VECTOR pos_;

	// 向いている方向
	float angle_;

	// 回転速度
	float rotSpeed_;

	// HP
	int hp_;

	// 無敵時間
	int damageFrame_;

	// 死んでいるか
	bool isDead_;

	const char* fileName_;
};