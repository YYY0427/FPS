#pragma once
#include "Scene.h"
#include <memory>
#include <vector>
#include <DxLib.h>

class Camera;
class Player;
class Shot;
class EnemyManager;
class SkyDoom;
class Tower;
class Collision;
class StageManager;
class EnemyShotFactory;

/// <summary>
/// メインシーン
/// </summary>
class MainScene : public Scene
{
public:
	MainScene(SceneManager& manager, StageManager* pStageManager);
	virtual ~MainScene();

	void Update(const InputState& input);
	void Draw();

	// ショットを撃つ
	void StartPlayerShot(VECTOR pos, VECTOR vec);

	// レティクルの位置の取得
	float GetReticlePosX() const;
	float GetReticlePosY() const;

	// プレイヤーがステージから落ちたときのフェード
	void PlayerFallFade();
private:
	// フェードアウト
	void FadeOutUpdate(const InputState& input);

	// フェードイン
	void FadeInUpdate();

	// 通常のアップデート
	void NormalUpdate(const InputState& input);

private:
	// メンバ関数ポインタ
	using UpdateFunc_t = void (MainScene::*) (const InputState& input);
	UpdateFunc_t updateFunc_;

	// ポインタ
	std::shared_ptr<Camera> pCamera_ ;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::vector<std::shared_ptr<Shot>> pShot_;
	std::shared_ptr<SkyDoom> pSkyDoom_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<Collision> pCollision_;
	std::shared_ptr<EnemyShotFactory> pEnemyShotFactory_;
	StageManager* pStageManager_;	 // シーンがなくなっても消えてほしくないのでshard_ptrを使わない

	// 画像ハンドル
	int gameOverUIhandle_;
	int gameClearUIhandle_;

	// フェイド
	int fadeTimer_;
	int fadeValue_;
	int gameOverUIfadeValue_;
	int gameOverUIFadeTimer_;

	// シャドウマップ
	int shadowMap_;

	// ゲームオーバーかどうか
	bool isGameOver_;
};