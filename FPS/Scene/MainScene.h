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
class FieldManager;
class Tower;
class Collision;

/// <summary>
/// メインシーン
/// </summary>
class MainScene : public Scene
{
public:
	MainScene(SceneManager& manager);
	virtual ~MainScene();

	void Init();
	void Update(const InputState& input);
	void Draw();

	// ショットを撃つ
	void StartShot(VECTOR pos, VECTOR vec);

	// レティクルの位置の取得
	float GetReticlePosX() const;
	float GetReticlePosY() const;

private:
	void FadeInUpdate(const InputState& input);
	void NormalUpdate(const InputState& input);
	void FadeOutUpdate(const InputState& input);

private:
	// メンバ関数ポインタ
	using UpdateFunc_t = void (MainScene::*) (const InputState& input);
	UpdateFunc_t updateFunc_;

	// ポインタ
	std::shared_ptr<Camera> pCamera_ ;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<FieldManager> pFieldManager_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::vector<std::shared_ptr<Shot>> pShot_;
	std::shared_ptr<SkyDoom> pSkyDoom_;
	std::shared_ptr<Tower> pTower_;
	Collision* pCollision_;

	// ゲームオーバー時のUI用ハンドル
	int gameOverUIhandle_;
	int gameOverFadeTimer_;

	// フェイド
	int fadeTimer_;
	int fadeValue_;
	int gameOverUIfadeValue_;

	// シャドウマップ
	int shadowMap_;
};