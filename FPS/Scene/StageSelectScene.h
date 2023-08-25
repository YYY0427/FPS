#pragma once
#include <memory>
#include "Scene.h"

class StageManager;

/// <summary>
/// �X�e�[�W��I������V�[��
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