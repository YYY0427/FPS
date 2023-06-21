#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;

class EnemyBase
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	void Init();
	virtual void Update() = 0;
	void Draw();

	// 2DのUIの表示
	void DrawUI();

	// ダメージを受けた
	virtual void OnDamage(int damage) = 0;

	// モデルのハンドルのゲッター
	int GetModelHandle() const;

	// 当たり判定のフレームインデックスのゲッター
	int GetColFrameIndex() const;

	// 位置のゲッター
	VECTOR GetPos() const { return pos_; }

	// 死亡情報のゲッター
	bool GetDead() const { return isDead_; }

	// プレイヤーのセッター
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// 当たり判定の半径のゲッター
	float GetColRadius() const { return colRadius_; };

protected:
	// プレイヤーが正面にいるかどうか
	bool IsPlayerFront() const;

	// プレイヤーに向かう
	virtual void UpdateToPlayer() = 0;

	// 前に移動する(プレイヤーが見えていない)
	virtual void UpdateToFront() = 0;

	// 方向転換を行う(プレイヤーが見えていない)
	virtual void UpdateTurn() = 0;

	// 弾が当たったときのアニメーション
	virtual void UpdateHitDamage() = 0;

	// 死亡アニメーション
	virtual void UpdateDead();

protected:
	// ポインタ
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Model> pModel_;

	// 現在再生しているアニメーション番号
	int animNo_ = 0;

	// アニメーション番号
	int deadAnimNo_ = 0;

	// フレームカウント
	int frameCount_ = 0;

	// 位置
	VECTOR pos_ = VGet(0, 0, 0);

	// プレイヤーからの距離
	VECTOR toPlayer_ = VGet(0, 0, 0);

	// 向いている方向
	float angle_ = 0.0f;

	// 回転速度
	float rotSpeed_ = 0.0f;

	// HP
	int hp_ = 0;

	// 最大HP
	int maxHp_ = 0;

	// HPバーの横幅
	int hpBarWidth_ = 0;

	// HPバーの縦幅
	int hpBarHeight_ = 0;

	// 無敵時間
	int damageFrame_ = 0;

	// 死んでいるか
	bool isDead_ = false;

	// モデルハンドルのファイル
	const char* fileName_ = nullptr;

	// 当たり判定の半径
	float colRadius_ = 0.0f;

	// 視野角
	float viewAngle_ = 0.0f;

	// キャラクターの向いている方向
	VECTOR dir_ = VGet(0, 0, 0);

	// 敵が死んでから消えるまでの時間
	int deadDisappearTime_ = 0;
};