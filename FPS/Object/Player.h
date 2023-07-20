#pragma once
#include <DxLib.h>
#include <memory>
#include <vector>
#include "../Model.h"

class InputState;
class Model;
class Camera;
class MainScene;
class Collision;
class Tower;

class Player
{
public:
	Player(MainScene* pMainScene);
	virtual ~Player();

	void Update(const InputState& input);
	void Draw();

	void SetRespawn();

	// カメラのポインタのセッター
	void SetCamera(std::shared_ptr<Camera> pCamera) { pCamera_ = pCamera; }

	// 当たり判定ポインタのセッター
	void SetCollision(std::shared_ptr<Collision> pCollision) { pCollision_ = pCollision; }

	// タワーのセッター
	void SetTower(std::shared_ptr<Tower> pTower) { pTower_ = pTower; }

	// プレイヤーが落下したかのゲッター
	bool IsFall() const { return isFall_; }

	// プレイヤーの位置のゲッター
	VECTOR GetPos() const { return pos_; }

	// モデルハンドルのゲッター
	int GetHandle() const { return pModel_->GetModelHandle(); }

	// プレイヤーのHPのゲッター
	int GetHP() const { return hp_; }

	// プレイヤーが死んでいるかのゲッター
	bool GetIsDead() const { return isDead_; }

	// 当たり判定の半径のゲッター
	float GetColRadius() const;

	// ダメージを受けた
	void OnDamage(int damage);

	// FPSとTPS切り替え用のフレームの表示非表示
	void SetVisible(bool visible);

	void SetJump(bool jump) { isJump_ = jump; }

private:
	// 待機
	void UpdateIdle(const InputState& input);

	// ショットを撃つ
	void UpdateIdleShot(const InputState& input);

	// 死亡アニメーション
	void UpdateDead(const InputState& input);

	// ダメージアニメーション
	void UpdateOnDamage(const InputState& input);

private:
	// メンバー関数ポインタ
	void(Player::* updateFunc_)(const InputState& input);

	// メインシーン
	MainScene* pMainScene_;

	std::shared_ptr<Collision> pCollision_;

	// プレイヤーのモデル
	std::shared_ptr<Model> pModel_;

	std::shared_ptr<Camera> pCamera_;

	std::shared_ptr<Tower> pTower_;

	// 再生しているアニメーション番号
	int animNo_;

	// フレームカウント
	int frameCount_;

	// プレイヤーの位置
	VECTOR pos_;

	// ジャンプ処理用加速度
	float jumpAcc_;

	// HP
	int hp_;

	// 無敵時間
	int damageFrame_;

	// 移動中か
	bool isMoving_;

	// プレイヤーが死んでいるか
	bool isDead_;

	// ショットを撃ってから何フレーム経過したかのカウント
	int shotFrameCount_;

	// 移動ベクトル
	VECTOR moveVec_;

	// ジャンプ中か
	bool isJump_;

	// 地面から落ちたか
	bool isFall_;
};