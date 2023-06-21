#pragma once
#include <DxLib.h>
#include <memory>
#include "../Model.h"

class InputState;
class Model;
class Camera;
class MainScene;

class Player
{
public:
	Player();
	virtual ~Player();

	void Init();
	void Update(const InputState& input);
	void Draw();

	// MainSceneのポインタのセッター
	void SetMainScene(std::shared_ptr<MainScene>pMainScene) { pMainScene_ = pMainScene; }

	// プレイヤーの位置のゲッター
	VECTOR GetPos() const { return pos_; }

	// プレイヤーの1フレーム前の位置のゲッター
	VECTOR Get1FramePastPos() const { return; }

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

	// カメラのポインタのセッター
	void SetCamera(std::shared_ptr<Camera> pCamera) { pCamera_ = pCamera; }

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
	std::shared_ptr<MainScene> pMainScene_;

	// プレイヤーのモデル
	std::shared_ptr<Model> pModel_;

	std::shared_ptr<Camera> pCamera_;

	// 再生しているアニメーション番号
	int animNo_;

	// フレームカウント
	int frameCount_;

	// プレイヤーの位置
	VECTOR pos_;

	// プレイヤーの1フレーム前の位置
	VECTOR pastPos_;

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
};