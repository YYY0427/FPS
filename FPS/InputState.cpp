#include "InputState.h"
#include <DxLib.h>

InputState::InputState()
{
	defaultMapTable_[InputType::next] = { {InputCategory::keybd, KEY_INPUT_RETURN},
										{InputCategory::pad, PAD_INPUT_R },			//スタートボタン
										{InputCategory::mouse, MOUSE_INPUT_LEFT } };

	defaultMapTable_[InputType::prev] = { {InputCategory::keybd, KEY_INPUT_SPACE},
										{InputCategory::pad, PAD_INPUT_7} };		//バックボタン

	defaultMapTable_[InputType::pause] = { {InputCategory::keybd, KEY_INPUT_P},
										{InputCategory::pad, PAD_INPUT_L } };		//セレクトボタン


	defaultMapTable_[InputType::keyconf] = { {InputCategory::keybd, KEY_INPUT_K},
										{InputCategory::pad, PAD_INPUT_Y } };		//左ショルダー

	defaultMapTable_[InputType::change] = { {InputCategory::keybd, KEY_INPUT_C},
										{InputCategory::pad, PAD_INPUT_Z } };		//右ショルダー

	defaultMapTable_[InputType::up] = { {InputCategory::keybd, KEY_INPUT_UP},
										{InputCategory::pad, PAD_INPUT_UP } };		//↑

	defaultMapTable_[InputType::down] = { {InputCategory::keybd, KEY_INPUT_DOWN},
										{InputCategory::pad, PAD_INPUT_DOWN } };	//↓

	defaultMapTable_[InputType::right] = { {InputCategory::keybd, KEY_INPUT_RIGHT},
										{InputCategory::pad, PAD_INPUT_RIGHT } };	//→

	defaultMapTable_[InputType::left] = { {InputCategory::keybd, KEY_INPUT_LEFT},
										{InputCategory::pad, PAD_INPUT_LEFT } };	//←

	defaultMapTable_[InputType::shot] = { {InputCategory::keybd, KEY_INPUT_Z},
										{InputCategory::pad, PAD_INPUT_C } };		//ショット

	defaultMapTable_[InputType::rapid] = { {InputCategory::keybd, KEY_INPUT_A},
									{InputCategory::pad, PAD_INPUT_A } };			//連射

	defaultMapTable_[InputType::switcing] = { {InputCategory::keybd, KEY_INPUT_X},
									{InputCategory::pad, PAD_INPUT_X } };			//


	inputMapTable_ = defaultMapTable_;

	LoadKeyInfo();

	//一時マップテーブルにコピー
	tempMapTable_ = inputMapTable_;

	//入力タイプの名前のテーブルを作る
	inputNameTable_[InputType::next] = "next";
	inputNameTable_[InputType::prev] = "prev";
	inputNameTable_[InputType::pause] = "pause";
	inputNameTable_[InputType::keyconf] = "keyconf";
	inputNameTable_[InputType::change] = "change";
	inputNameTable_[InputType::shot] = "shot";
	inputNameTable_[InputType::rapid] = "rapid";
	inputNameTable_[InputType::switcing] = "switcihg";

	currentInput_.resize(static_cast<int>(InputType::max));
	lastInput_.resize(static_cast<int>(InputType::max));
}

void InputState::Update()
{
	lastInput_ = currentInput_;	//直前の入力情報を記憶しておく

	char keystate[256];
	GetHitKeyStateAll(keystate); //全キー情報取得

	int padState = GetJoypadInputState(DX_INPUT_PAD1);	//パッド1コンの情報を取得する
	int mouseState = GetMouseInput();

	//マップの全情報をループする
	for (const auto& keymap : inputMapTable_)
	{
		//入力情報配列をループする
		for (const auto& input : keymap.second)
		{
			//このinputの中身は、keybd,KEY_INPUT_RETURNなどのセット(InputInfo)が入っている
			//keymap.secondは、この入力情報セットInputInfoが入っている
			//keymap.firstは、対応するゲーム入力名の"InputType::next"などが入っている
			if (input.cat == InputCategory::keybd)
			{
				currentInput_[static_cast<int>(keymap.first)] = keystate[input.id];
			}
			else if (input.cat == InputCategory::pad)
			{
				currentInput_[static_cast<int>(keymap.first)] = padState & input.id;
			}
			else if (input.cat == InputCategory::mouse)
			{
				currentInput_[static_cast<int>(keymap.first)] = mouseState & input.id;
			}
			//3つの入力のうちどれかがtrueだったらもう「入力されてる」
			//とみなして、breakする。
			if (currentInput_[static_cast<int>(keymap.first)])
			{
				break;
			}
		}
	}
}

void InputState::RewriteInputInfo(InputType type, InputCategory cat, int id)
{
	//入力種別(割り当て先)がなければ、無効なので無視します
	if (tempMapTable_.count(type) == 0)
	{
		return;
	}
	bool isRewrite = false;
	for (auto& inputInfo : tempMapTable_[type])
	{
		//カテゴリがヒットしたら
		if (inputInfo.cat == cat)
		{
			//IDを上書きする
			inputInfo.id = id;
			isRewrite = true;
			break;
		}
	}
	//もしカテゴリが存在しなかったら、ここで追加しておく
	if (!isRewrite)
	{
		tempMapTable_[type].push_back({ cat, id });
	}
}

void InputState::CommitChangedInputInfo()
{
	inputMapTable_ = tempMapTable_;
}

void InputState::RollbackChangedInputInfo()
{
	tempMapTable_ = inputMapTable_;
}

void InputState::ResetInputInfo()
{
	inputMapTable_ = defaultMapTable_;
	tempMapTable_ = defaultMapTable_;
}

void InputState::SaveKeyInfo() const
{
	FILE* fp = nullptr;

	auto err = fopen_s(&fp, "key.info", "wb");
	if (fp == nullptr)
	{
		return;
	}

	//仮想キータイプの数を書き込む
	int keytypeNum = static_cast<int>(inputMapTable_.size());
	fwrite(&keytypeNum, sizeof(keytypeNum), 1, fp);

	//仮想キータイプ(next, prevなど)のループ
	for (const auto& key : inputMapTable_)
	{
		int keytype = static_cast<int>(key.first);

		//仮想キー番号
		fwrite(&keytype, sizeof(keytype), 1, fp);
		int dataSize = static_cast<int>(key.second.size());

		//いくつ実入力データがあるのか
		fwrite(&dataSize, sizeof(dataSize), 1, fp);

		//vector型のdata()は先頭のアドレスを返す
		//一気にその入力のデータを書き込む
		fwrite(key.second.data(), dataSize * sizeof(InputInfo), 1, fp);
	}
	fclose(fp);
}

void InputState::LoadKeyInfo()
{
	int handle = FileRead_open("key.info");

	if (handle == 0)
	{
		return;
	}

	int keyTypeNum = 0;

	FileRead_read(&keyTypeNum, sizeof(keyTypeNum), handle);
	inputMapTable_.clear();

	for (int i = 0; i < keyTypeNum; i++)
	{
		int  inputType;
		FileRead_read(&inputType, sizeof(inputType), handle);

		int dataSize = 0;
		FileRead_read(&dataSize, sizeof(dataSize), handle);
		std::vector<InputInfo> inputInfoes(dataSize);
		FileRead_read(inputInfoes.data(), sizeof(InputInfo) * dataSize, handle);
		inputMapTable_[static_cast<InputType>(inputType)] = inputInfoes;
	}
	tempMapTable_ = inputMapTable_;
	FileRead_close(handle);
}

bool InputState::IsPressed(InputType type) const
{
	return currentInput_[static_cast<int>(type)];
}

bool InputState::IsTriggered(InputType type) const
{
	return IsPressed(type) && !lastInput_[static_cast<int>(type)];
}
