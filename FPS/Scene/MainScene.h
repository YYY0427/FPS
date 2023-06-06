#pragma once
#include "Scene.h"
#include <memory>
#include <vector>
#include <DxLib.h>

class Camera;
class Player;
class Field;
class Shot;
class EnemyManager;

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
	std::shared_ptr<Camera> pCamera_ = nullptr;
	std::shared_ptr<Player> pPlayer_ = nullptr;
	std::shared_ptr<Field> pField_ = nullptr;
	std::shared_ptr<EnemyManager> pEnemyManager_ = nullptr;
	std::vector<std::shared_ptr<Shot>> pShot_;

	// フェイド
	int fadeTimer_;
	int fadeValue_;

	// シャドウマップ
	int shadowMap_;

	using UpdateFunc_t = void (MainScene::*) (const InputState& input);
	UpdateFunc_t updateFunc_ = nullptr;

	void FadeInUpdate(const InputState& input);
	void NormalUpdate(const InputState& input);
	void FadeOutUpdate(const InputState& input);
};