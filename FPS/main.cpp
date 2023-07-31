#include <DxLib.h>
#include "Game.h"
#include "InputState.h"
#include "SoundManager.h"
#include "Scene/SceneManager.h"
#include "Scene/TitleScene.h"
#include "ThreeDimensionEffectManager.h"

// プログラムは WinMain から始まります
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// Windowモード設定
	ChangeWindowMode(Game::window_mode);

	// Window名設定
	SetMainWindowText(Game::title_text);

	// 画面サイズの設定
	SetGraphMode(Game::screen_width, Game::screen_height, Game::color_depth);

	// ゲーム中にウィンドウモードを切り替えてもグラフィックハンドルをリセットしない
	SetChangeScreenModeGraphicsSystemResetFlag(false);

	// ほかのウィンドウを選択していても動くようにする
	SetAlwaysRunFlag(false);

	// ウィンドウのサイズを変更可能にする
	SetWindowSizeChangeEnableFlag(true);

	// DirectX9を使用するようにする。(DirectX11も可)
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_9);

	// ********** フォントのロード **********
	LPCSTR font_path = "Data/Font/nicoca_v2.ttf"; // 読み込むフォントファイルのパス
	if (AddFontResourceEx(font_path, FR_PRIVATE, NULL) < 0)
	{
		return -1;
	}
	
	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)
	{
		// エラーが起きたら直ちに終了
		return -1;
	}

	// ダブルバッファモード
	// 裏画面に描画
	SetDrawScreen(DX_SCREEN_BACK);

	auto& effectManager = ThreeDimensionEffectManager::GetInstance();
	auto& soundManager = SoundManager::GetInstance();
	InputState input;
	SceneManager sceneManager;

	if (effectManager.Init() == -1)
	{
		return -1;
	}

	sceneManager.ChangeScene(new TitleScene(sceneManager));

	// 異常が起きた時に終了
	while (ProcessMessage() == 0)
	{
		LONGLONG time = GetNowHiPerformanceCount();

		// 画面のクリア
		ClearDrawScreen();

		input.Update();
		sceneManager.Update(input);
		effectManager.Update();

		sceneManager.Draw();
		effectManager.Draw();

		// FPS(Frame Per Second)の取得
		auto fps = GetFPS();

		// 描画命令数の取得
		auto drawcall = GetDrawCallCount();

		DrawFormatString(10, 30, 0x000000, "FPS = %2.2f", fps);
		DrawFormatString(10, 60, 0x000000, "DC = %d", drawcall);

		// 裏画面を表画面を入れ替える
		ScreenFlip();

		// escキーを押したら終了する
		if (CheckHitKey(KEY_INPUT_ESCAPE))	break;

		// fpsを60に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{

		}
	}

	effectManager.End();

	// ＤＸライブラリ使用の終了処理
	DxLib_End();

	// ソフトの終了 
	return 0;
}
