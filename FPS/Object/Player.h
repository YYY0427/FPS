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

	// MainSceneのポインタの取得
	void SetMainScene(std::shared_ptr<MainScene>pMainScene) { pMainScene_ = pMainScene; }

	// プレイヤーの位置を取得する
	VECTOR GetPos() const { return pos_; }

	// モデルハンドルの取得
	int GetHandle() const { return pModel_->GetModelHandle(); }

	// プレイヤーのHPの取得
	int GetHP() const { return hp_; }

	// プレイヤーが死んでいるかの取得
	bool GetIsDead() const { return isDead_; }

	// 当たり判定の半径
	float GetColRadius();

	// ダメージを受けた
	void OnDamage(int damage);

	// FPSとTPS切り替え用のフレームの表示非表示
	void SetVisible(bool visible);

	// カメラのポインタの取得
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
};