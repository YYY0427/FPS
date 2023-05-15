#pragma once
#include "Scene.h"

/// <summary>
/// ƒƒCƒ“ƒV[ƒ“
/// </summary>
class MainScene : public Scene
{
public:
	MainScene(SceneManager& manager);
	virtual ~MainScene();

	void Update(const InputState& input);
	void Draw();

private:
	int fadeTimer_ = 0;
	int fadeValue_ = 255;

	using UpdateFunc_t = void (MainScene::*) (const InputState& input);
	UpdateFunc_t updateFunc_ = nullptr;

	void FadeInUpdate(const InputState& input);
	void NormalUpdate(const InputState& input);
	void FadeOutUpdate(const InputState& input);
};