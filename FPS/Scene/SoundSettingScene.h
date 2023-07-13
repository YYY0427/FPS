#pragma once
#include "Scene.h"

/// <summary>
/// サウンド設定シーン
/// </summary>
class SoundSettingScene : public Scene
{
public:
	SoundSettingScene(SceneManager& manager);
	virtual ~SoundSettingScene();

	void Update(const InputState& input);
	void Draw();

private:
	int currentInputIndex_;
	unsigned int bgmCategoryColor_;
	unsigned int seCategoryColor_;
};