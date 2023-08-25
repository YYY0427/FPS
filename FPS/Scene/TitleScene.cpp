#include <DxLib.h>
#include "TitleScene.h"
#include "SceneManager.h"
#include "MainScene.h"
#include "../InputState.h"
#include "../SoundManager.h"
#include "../DrawFunctions.h"
#include "../Game.h"
#include "StageSelectScene.h"

namespace
{
	// フェードの速度
	constexpr int fade_interval = 30;
}

void TitleScene::FadeInUpdate(const InputState& input)
{
	fadeValue_ = static_cast<int>(255 * (static_cast<float>(fadeTimer_) / 
		static_cast<float>(fade_interval)));
	if (--fadeTimer_ == 0)
	{
		updateFunc_ = &TitleScene::NormalUpdate;
		fadeValue_ = 0;
	}
}

void TitleScene::NormalUpdate(const InputState& input)
{
	//次へのボタンが押されたら次のシーンへ行く
	if (input.IsTriggered(InputType::next))
	{
		updateFunc_ = &TitleScene::FadeOutUpdate;
	}
}

void TitleScene::FadeOutUpdate(const InputState& input)
{
	fadeValue_ = static_cast<int>(255 * (static_cast<float>(fadeTimer_) / static_cast<float>(fade_interval)));
	if (++fadeTimer_ == fade_interval)
	{
		manager_.ChangeScene(new StageSelectScene(manager_));
		return;
	}
}

TitleScene::TitleScene(SceneManager& manager) :
	Scene(manager),
	updateFunc_(&TitleScene::FadeInUpdate),
	fadeTimer_(fade_interval)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update(const InputState& input)
{
	(this->*updateFunc_)(input);
}

void TitleScene::Draw(const InputState& input)
{
	DrawString(0, 0, "TitleScene", 0xffffff, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeValue_);
	DrawBox(0, 0, Game::screen_width, Game::screen_height, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}