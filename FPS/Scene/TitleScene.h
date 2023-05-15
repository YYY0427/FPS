#pragma once
#include "Scene.h"

class InputState;

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public Scene
{
public:
	TitleScene(SceneManager& manager);
	~TitleScene();

	void Update(const InputState& input);
	void Draw();

private:
	int fadeTimer_;			// フェードタイマー
	int fadeValue_ = 255;	// 黒矩形とのブレンド具合

	// フェードインの時のUpdate関数
	void FadeInUpdate(const InputState& input);

	// 通常状態のUpdate関数
	void NormalUpdate(const InputState& input);

	// フェードアウトの時のUpdate関数
	void FadeOutUpdate(const InputState& input);

	// Update用メンバ関数ポインタ
	void (TitleScene::* updateFunc_)(const InputState& input);
};