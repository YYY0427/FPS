#pragma once
#include "Scene.h"

class InputState;
class SceneManager;

/// <summary>
/// ポーズシーン
/// </summary>
class PauseScene : public Scene
{
public:
	PauseScene(SceneManager& manager);
	virtual ~PauseScene();

	virtual void Update(const InputState& input)override;
	void Draw();

private:
	unsigned int keyConfigCategoryColor_ = 0x000000;
	unsigned int soundSettingCategoryColor_ = 0x000000;

	int currentInputIndex_ = 0;
};

