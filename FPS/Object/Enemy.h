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

	// ダメージを受けた
	void OnDamage(int damage);

	// モデルのハンドルのゲッター
	int GetModelHandle() const;

	// 当たり判定のフレームインデックスのゲッター
	int GetColFrameIndex() const;

	// 位置のゲッター
	VECTOR GetPos() const { return pos_; }

	// 死亡情報のゲッター
	bool GetDead() const { return isDead_; }

	// 当たり判定の半径のゲッター
	float GetColRadius() const;

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

	// モデルハンドルのファイル
	const char* fileName_;
};