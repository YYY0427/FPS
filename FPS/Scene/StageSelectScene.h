#pragma once
#include <memory>
#include "Scene.h"
//#include "../StageManager.h"

class StageManager;

class StageSelectScene : public Scene
{
public:
	StageSelectScene(SceneManager& manager);
	virtual ~StageSelectScene();

	void Init();
	void Update(const InputState& input);
	void Draw();

private:
	StageManager* pStageManager_;
	int currentInputIndex_;
};