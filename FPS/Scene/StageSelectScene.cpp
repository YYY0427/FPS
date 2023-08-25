#include "StageSelectScene.h"
#include "MainScene.h"
#include "SceneManager.h"
#include "../InputState.h"
#include "../StageManager.h"
#include "../Game.h"

namespace
{
	// ‘I‘ðŽˆ‚Ì”
	constexpr int option_num = 1;
}

StageSelectScene::StageSelectScene(SceneManager& manager) :
	Scene(manager),
	currentInputIndex_(0),
	pStageManager_(nullptr)
{
	Init();
}

StageSelectScene::~StageSelectScene()
{
}

void StageSelectScene::Init()
{
	pStageManager_ = new StageManager();
}

void StageSelectScene::Update(const InputState& input)
{
	//”Žš‚ð‰ñ‚·ˆ—
	if (input.IsTriggered(InputType::up))
	{
		currentInputIndex_ = ((currentInputIndex_ - 1) + option_num) % option_num;
	}
	else if (input.IsTriggered(InputType::down))
	{
		currentInputIndex_ = (currentInputIndex_ + 1) % option_num;
	}

	// Œˆ’è‚ª‰Ÿ‚³‚ê‚½‚Æ‚«ƒV[ƒ“‘JˆÚ
	if (input.IsTriggered(InputType::next))
	{
		pStageManager_->StageSelect(currentInputIndex_);
		manager_.ChangeScene(new MainScene(manager_, pStageManager_));
		return;
	}
}

void StageSelectScene::Draw(const InputState& input)
{
	if (currentInputIndex_ == 0)
	{
		DrawFormatString(Game::screen_width / 2, Game::screen_height / 2, 0xffffff, "stage1");
	}
	else if (currentInputIndex_ == 1)
	{
		DrawFormatString(Game::screen_width / 2, Game::screen_height / 2, 0xffffff, "stage2");
	}
}