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
	//ã‰º‚Å‰ñ‚éˆ—
	if (input.IsTriggered(InputType::up))
	{
		currentInputIndex_ = ((currentInputIndex_ - 1) + option_num) % option_num;
	}
	else if (input.IsTriggered(InputType::down))
	{
		currentInputIndex_ = (currentInputIndex_ + 1) % option_num;
	}

	// Œˆ’è‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
	if (input.IsTriggered(InputType::next))
	{
		switch (currentInputIndex_)
		{
		case 0:
			pStageManager_->StageSelect(currentInputIndex_);
			break;
		case 1:

			break;
		}
		manager_.ChangeScene(new MainScene(manager_, pStageManager_));
		return;
	}
}

void StageSelectScene::Draw()
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