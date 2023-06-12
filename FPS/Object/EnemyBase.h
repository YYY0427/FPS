#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;
class MainScene;

class EnemyBase
{
public:
	EnemyBase();
	virtual ~EnemyBase(){}

	void Init();
	virtual void Update() = 0;
	void Draw();

	// 2DのUIの表示
	virtual void DrawUI() = 0;

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

	// メインシーンのセッター
	void SetMainScene(std::shared_ptr<MainScene> pMainScene) { pMainScene_ = pMainScene; }

	// プレイヤーのセッター
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// 当たり判定の半径のゲッター
	virtual float GetColRadius() const = 0;

protected:
	// プレイヤーが正面にいるかどうか
	virtual bool IsPlayerFront() const = 0;

	// プレイヤーに向かう
	virtual void UpdateToPlayer() = 0;

	// 前に移動する(プレイヤーが見えていない)
	virtual void UpdateToFront() = 0;

	// 方向転換を行う(プレイヤーが見えていない)
	virtual void UpdateTurn() = 0;

	// 弾が当たったときのアニメーション
	virtual void UpdateHitDamage() = 0;

	// 死亡アニメーション
	virtual void UpdateDead() = 0;

protected:
	// ポインタ
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<MainScene> pMainScene_;

	// 再生しているアニメーション番号
	int animNo_ = 0;

	// フレームカウント
	int frameCount_ = 0;

	// 位置
	VECTOR pos_ = VGet(0, 0, 0);

	// 向いている方向
	float angle_ = 0.0f;

	// 回転速度
	float rotSpeed_ = 0.0f;

	// HP
	int hp_ = 0;

	// 無敵時間
	int damageFrame_ = 0;

	// 死んでいるか
	bool isDead_ = false;

	// モデルハンドルのファイル
	const char* fileName_ = nullptr;
};