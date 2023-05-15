#include "KeyConfigScene.h"
#include "SceneManager.h"
#include "../InputState.h"
#include "../Game.h"
#include <DxLib.h>

KeyConfigScene::KeyConfigScene(SceneManager& manager, const InputState& input) :
	Scene(manager),
	inputState_(input)
{

}

KeyConfigScene::~KeyConfigScene()
{
	inputState_.SaveKeyInfo();
}

void KeyConfigScene::Update(const InputState& input)
{
	// constはがし
	auto& configInput = const_cast<InputState&>(input);
	if (!isEditing_)
	{
		if (input.IsTriggered(InputType::prev))
		{
			configInput.RollbackChangedInputInfo();
			manager_.PopScene();
			return;
		}
		if (input.IsTriggered(InputType::change))
		{
			configInput.RewriteInputInfo(InputType::prev, InputCategory::keybd, KEY_INPUT_ESCAPE);
			configInput.RewriteInputInfo(InputType::prev, InputCategory::pad, PAD_INPUT_A);
			configInput.RewriteInputInfo(InputType::prev, InputCategory::mouse, MOUSE_INPUT_RIGHT);

			// 何回キーを書き換えられたか
			static int count = 0;
			++count;
			char logstr[64] = {};
			sprintf_s(logstr, sizeof(logstr), "%d回キーが書き換えられました\n", count);

			// 出力ログに表示
			OutputDebugStringA(logstr);
		}

		const int nameCount = static_cast<int>(input.inputNameTable_.size()) + 2;

		// 上下で回る処理
		if (input.IsTriggered(InputType::up))
		{
			currentInputIndex_ = ((currentInputIndex_ - 1) + nameCount) % nameCount;
		}
		else if (input.IsTriggered(InputType::down))
		{
			currentInputIndex_ = (currentInputIndex_ + 1) % nameCount;
		}
	}

	// この時はもう、確定しますを選択している
	if (currentInputIndex_ == input.inputNameTable_.size())
	{
		if (input.IsTriggered(InputType::next))
		{
			configInput.CommitChangedInputInfo();
			manager_.PopScene();
			return;
		}
	}
	if (currentInputIndex_ == input.inputNameTable_.size() + 1)
	{
		if (input.IsTriggered(InputType::next))
		{
			configInput.ResetInputInfo();
			return;
		}
	}

	// nextボタンでエディット中かそうじゃなかを決定する
	if (input.IsTriggered(InputType::next))
	{
		isEditing_ = !isEditing_;
		return;
	}

	if (isEditing_)
	{
		char keystate[256];
		GetHitKeyStateAll(keystate);
		auto padState = GetJoypadInputState(DX_INPUT_PAD1);
		auto mouseState = GetMouseInput();

		int idx = 0;
		InputType currentType = InputType::max;
		for (const auto& name : inputState_.inputNameTable_)
		{
			if (currentInputIndex_ == idx)
			{
				currentType = name.first;
				break;
			}
			idx++;
		}

		// キーの入れ替え
		for (int i = 0; i < 256; ++i)
		{
			if (keystate[i])
			{
				configInput.RewriteInputInfo(currentType, InputCategory::keybd, i);
				break;
			}
		}
		if (padState != 0)
		{
			configInput.RewriteInputInfo(currentType, InputCategory::pad, padState);
		}
		if (mouseState != 0)
		{
			configInput.RewriteInputInfo(currentType, InputCategory::mouse, mouseState);
		}
	}
}

void KeyConfigScene::Draw()
{
	constexpr int pw_width = 700;	// キーコンフィグ枠の幅
	constexpr int pw_height = 500;	// キーコンフィグ枠の高さ
	constexpr int pw_start_x = (Game::screen_width - pw_width) / 2 + 50;	// キーコンフィグ枠の左
	constexpr int pw_start_y = (Game::screen_height - pw_height) / 2 + 50;	// キーコンフィグ枠上

	// キーコンフィグウィンドウ背景
	DrawBox(pw_start_x, pw_start_y, pw_start_x + pw_width, pw_start_y + pw_height, 0x008800, true);

	// キーコンフィグ中メッセージ
	DrawString(pw_start_x + 10, pw_start_y + 10, "keyconfig...", 0xffffaa);

	// 各キーを並べて表示
	auto y = pw_start_y + 30;
	int idx = 0;
	bool isInputTypeSelected = false;
	for (const auto& name : inputState_.inputNameTable_)
	{
		int offset = 0;
		unsigned int color = 0xffffff;

		if (currentInputIndex_ == idx)
		{
			offset = 10;
			isInputTypeSelected = true;
			if (isEditing_)
			{
				color = 0xff0000;
			}
			DrawString(pw_start_x + 10, y, ">", 0xff0000);
		}

		//各キーの表示
		int x = pw_start_x + 20 + offset;
		DrawString(x, y, name.second.c_str(), color);

		auto type = name.first;
		auto it = inputState_.tempMapTable_.find(type);
		x += 64;
		DrawString(x, y, ":", color);


		for (const auto& elem : it->second)
		{
			x += 10;
			if (elem.cat == InputCategory::keybd)
			{
				DrawFormatString(x, y, color, "key = %d", elem.id);
			}
			else if (elem.cat == InputCategory::pad)
			{
				DrawFormatString(x, y, color, "pad = %d", elem.id);
			}
			else if (elem.cat == InputCategory::mouse)
			{
				DrawFormatString(x, y, color, "mouse = %d", elem.id);
			}
			x += 100;
		}
		y += 18;
		++idx;
	}
	y += 20;

	if (!isInputTypeSelected)
	{
		int yoffset = 0;
		if (currentInputIndex_ == inputState_.inputNameTable_.size() + 1)
		{
			yoffset = 20;
		}
		DrawString(pw_start_x + 80, y + yoffset, ">", 0xff0000);
	}

	//各キーの表示
	DrawString(pw_start_x + 100, y, "確定します", 0xffffff);

	y += 20;
	DrawString(pw_start_x + 100, y, "キーリセット", 0xffffff);

	//キーコンフィグウィンドウ枠線
	DrawBox(pw_start_x, pw_start_y, pw_start_x + pw_width, pw_start_y + pw_height, 0xffffff, false);
}
