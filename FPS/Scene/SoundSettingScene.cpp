#include "SoundSettingScene.h"
#include "SceneManager.h"
#include "../InputState.h"
#include "../SoundManager.h"
#include "../Game.h"
#include <DxLib.h>

namespace
{
	// ウィンドウタイトル
	const char* const window_title = "SoundSetting";

	// ボリュームを何個に分けるか
	constexpr int volume_divide_num = 10;
	
	// 音量設定ウィンドウ
	constexpr int window_width = 700;												// オプション枠の幅
	constexpr int window_height = 500;												// オプション枠の高さ
	constexpr int window_start_x = (Game::screen_width / 2) - (window_width / 2) + 50;	// オプション枠の左
	constexpr int window_start_y = (Game::screen_height / 2) - (window_height / 2) + 50;	// オプション枠上

	// 選択肢の数
	constexpr int name_count = 2;

	// いくつの値音量を変化させるか
	constexpr int volume_setting_num = 25;
}

SoundSettingScene::SoundSettingScene(SceneManager& manager) :
	Scene(manager)
{
}

SoundSettingScene::~SoundSettingScene()
{
}

void SoundSettingScene::Update(const InputState& input)
{
	//上下で回る処理
	if (input.IsTriggered(InputType::up))
	{
	//	SoundManager::GetInstance().Play("cursor");
		currentInputIndex_ = ((currentInputIndex_ - 1) + name_count) % name_count;
	}
	else if (input.IsTriggered(InputType::down))
	{
	//	SoundManager::GetInstance().Play("cursor");
		currentInputIndex_ = (currentInputIndex_ + 1) % name_count;
	}
	if (currentInputIndex_ == 0)
	{
		bgmCategoryColor = 0xff0000;
		seCategoryColor = 0x000000;
	}
	else 
	{
		bgmCategoryColor = 0x000000;
		seCategoryColor = 0xff0000;
	}

	auto& soundMgr = SoundManager::GetInstance();
	// BGM
	if (input.IsTriggered(InputType::right) && currentInputIndex_ == 0)
	{
		soundMgr.SetBGMVolume((std::min)(soundMgr.GetBGMVolume() + volume_setting_num, 255));
	}
	if (input.IsTriggered(InputType::left) && currentInputIndex_ == 0)
	{
		soundMgr.SetBGMVolume((std::max)(soundMgr.GetBGMVolume() - volume_setting_num, 0));
	}
	// SE
	if (input.IsTriggered(InputType::right) && currentInputIndex_ == 1)
	{
		soundMgr.SetSEVolume((std::min)(soundMgr.GetSEVolume() + volume_setting_num, 255));
	}
	if (input.IsTriggered(InputType::left) && currentInputIndex_ == 1)
	{
		soundMgr.SetSEVolume((std::max)(soundMgr.GetSEVolume() - volume_setting_num, 0));
	}

	// タイトル画面に戻る
	if (input.IsTriggered(InputType::prev))
	{
		soundMgr.SaveSoundConfig();
		manager_.PopScene();
	}
}

void SoundSettingScene::Draw()
{
	auto& soundMgr = SoundManager::GetInstance();

	// 音量の数の取得
	int bgmNum = soundMgr.GetBGMVolume() / volume_setting_num;
	int seNum = soundMgr.GetSEVolume() / volume_setting_num;

	// ウィンドウ
	DrawBox(window_start_x, window_start_y, window_start_x + window_width, window_start_y + window_height, GetColor(200, 200, 200), true);

	// ウィンドウタイトルの表示
	int width = GetDrawStringWidth(window_title, static_cast<int>(strlen(window_title)), false);
	DrawString(((window_width + window_start_x) / 2) - (width / 2), window_start_y + 50, window_title, 0x000000, false);

	// 項目名の表示
	DrawString(window_start_x + window_width / 2 - 250, 320, "BGM", bgmCategoryColor, false);
	DrawString(window_start_x + window_width / 2 - 250, 450, "SE", seCategoryColor, false);

	for (int i = 0; i < volume_divide_num; i++)
	{
		int width = i * 40;

		// 枠組み
		DrawRoundRect(window_start_x + window_width / 2 - 150 + width, 320, window_start_x + window_width / 2 - 150 + 30 + width, 320 + 80, 5, 5, GetColor(255, 165, 0), false);
		DrawRoundRect(window_start_x + window_width / 2 - 150 + width, 450, window_start_x + window_width / 2 - 150 + 30 + width, 450 + 80, 5, 5, GetColor(255, 165, 0), false);
	}

	// BGM
	for (int i = 0; i < bgmNum; i++)
	{
		int width = i * 40;
		DrawRoundRect(window_start_x + window_width / 2 - 150 + width, 320, window_start_x + window_width / 2 - 150 + 30 + width, 320 + 80, 5, 5, GetColor(255, 165, 0), true);
	}
	// SE
	for (int i = 0; i < seNum; i++)
	{
		int width = i * 40;
		DrawRoundRect(window_start_x + window_width / 2 - 150 + width, 450, window_start_x + window_width / 2 - 150 + 30 + width, 450 + 80, 5, 5, GetColor(255, 165, 0), true);
	}
}