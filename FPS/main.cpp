#include <DxLib.h>
#include <array>
#include <cassert>
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
	SetUseDirect3DVersion(DX_DIRECT3D_9EX);

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

	//VERTEX2DSHADER Vert[6];

	//int imgHandle = MakeScreen(Game::screen_width, Game::screen_height, true);
	//int psHandle = LoadPixelShader("SampleMono.pso");
	//assert(psHandle != -1);

	//// 頂点データの準備
	//Vert[0].pos = VGet(0.0f, 0.0f, 0.0f);
	//Vert[1].pos = VGet(Game::screen_width - 1, 0.0f, 0.0f);
	//Vert[2].pos = VGet(0.0f, Game::screen_height - 1, 0.0f);
	//Vert[3].pos = VGet(Game::screen_width - 1, Game::screen_height - 1, 0.0f);
	//Vert[0].dif = GetColorU8(255, 255, 255, 255);
	//Vert[0].spc = GetColorU8(0, 0, 0, 0);
	//Vert[0].u = 0.0f; Vert[0].v = 0.0f;
	//Vert[1].u = 1.0f; Vert[1].v = 0.0f;
	//Vert[2].u = 0.0f; Vert[2].v = 1.0f;
	//Vert[3].u = 1.0f; Vert[3].v = 1.0f;
	//Vert[0].su = 0.0f; Vert[0].sv = 0.0f;
	//Vert[1].su = 1.0f; Vert[1].sv = 0.0f;
	//Vert[2].su = 0.0f; Vert[2].sv = 1.0f;
	//Vert[3].su = 1.0f; Vert[3].sv = 1.0f;
	//Vert[0].rhw = 1.0f;
	//Vert[1].rhw = 1.0f;
	//Vert[2].rhw = 1.0f;
	//Vert[3].rhw = 1.0f;
	//Vert[4] = Vert[2];
	//Vert[5] = Vert[1];


	//SetUseTextureToShader(0, imgHandle);

	//SetUsePixelShader(psHandle);


	/*std::array< VERTEX2DSHADER, 4 > vertex;
	std::array< unsigned short, 6 > vertexIndex;

	auto Settings = [](VERTEX2DSHADER& vtx, float x, float y, float u, float v)
	{
		vtx.pos = VGet(x, y, 0.0f);
		vtx.u = u;
		vtx.v = v;
		vtx.su = u;
		vtx.sv = v;
		vtx.rhw = 1.0f;
		vtx.dif = GetColorU8(255, 255, 255, 255);
		vtx.spc = GetColorU8(0, 0, 0, 0);
	};*/

	//Settings(vertex[0], 100.0f, 100.0f, 0.0f, 0.0f);
	//Settings(vertex[1], 600.0f, 100.0f, 1.0f, 0.0f);
	//Settings(vertex[2], 100.0f, 600.0f, 0.0f, 1.0f);
	//Settings(vertex[3], 600.0f, 600.0f, 1.0f, 1.0f);

	//vertexIndex = { 0, 1, 2, 1, 2, 3 };

	
//	SetPSConstSF(GetConstIndexToShader("alpha", psHandle), 1.0f);
//	SetPSConstSF(GetConstIndexToShader("mosLv", psHandle), 0.5f);

//	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

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

	//	SetDrawScreen(imgHandle);
		sceneManager.Draw(input);
		effectManager.Draw();

	//	SetDrawScreen(DX_SCREEN_BACK);
		// 描画
	//	DrawPrimitive2DToShader(Vert, 6, DX_PRIMTYPE_TRIANGLELIST);
	//	DrawGraph(0, 0, imgHandle, true);

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
