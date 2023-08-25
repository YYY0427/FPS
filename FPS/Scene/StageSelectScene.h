#pragma once
#include <memory>
#include "Scene.h"

class StageManager;

/// <summary>
/// ステージを選択するシーン
/// </summary>
class StageSelectScene : public Scene
{
public:
	StageSelectScene(SceneManager& manager);
	virtual ~StageSelectScene();

	void Init();
	void Update(const InputState& input);
	void Draw(const InputState& input);

private:
	StageManager* pStageManager_;
	int currentInputIndex_;
};