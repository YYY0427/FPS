#pragma once
#include <DxLib.h>
#include <memory>

class InputState;
class Model;
class Camera;
class MainScene;
class Collision;
class Tower;

class PlayerBase
{
public:
	PlayerBase();
	virtual ~PlayerBase();

	void Update(const InputState& input);
	void Draw();

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
	int GetHandle() const;

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

protected:
	// メインシーン
	MainScene* pMainScene_;

	std::shared_ptr<Collision> pCollision_;

	// プレイヤーのモデル
	std::shared_ptr<Model> pModel_;

	std::shared_ptr<Camera> pCamera_;

	std::shared_ptr<Tower> pTower_;

	// 再生しているアニメーション番号
	int animNo_ = -1;

	// フレームカウント
	int frameCount_ = 0;

	// プレイヤーの位置
	VECTOR pos_ = VGet(0, 0, 0);

	// ジャンプ処理用加速度
	float jumpAcc_ = 0.0f;

	// HP
	int hp_ = 0;

	// 無敵時間
	int damageFrame_ = 0;

	// 移動中か
	bool isMoving_ = false;

	// プレイヤーが死んでいるか
	bool isDead_ = false;

	// ショットを撃ってから何フレーム経過したかのカウント
	int shotFrameCount_ = 0;

	// 移動ベクトル
	VECTOR moveVec_ = VGet(0, 0, 0);

	// ジャンプ中か
	bool isJump_ = false;

	// 地面から落ちたか
	bool isFall_ = false;
};
