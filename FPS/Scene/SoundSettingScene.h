#pragma once
#include "Scene.h"

/// <summary>
/// �T�E���h�ݒ�V�[��
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