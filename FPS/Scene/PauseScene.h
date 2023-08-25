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

	virtual void Update(const InputState& input) override;
	void Draw(const InputState& input);

private:
	unsigned int keyConfigCategoryColor_;
	unsigned int soundSettingCategoryColor_;

	int currentInputIndex_;
};

