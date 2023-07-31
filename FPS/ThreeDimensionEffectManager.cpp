#include "ThreeDimensionEffectManager.h"
#include "EffekseerForDXLib.h"
#include "DrawFunctions.h"
#include <cassert>

ThreeDimensionEffectManager::ThreeDimensionEffectManager()
{
	imgHandle_ = my::MyLoadGraph("Data/Effect/png.png");
}

ThreeDimensionEffectManager::~ThreeDimensionEffectManager()
{
}

int ThreeDimensionEffectManager::Init()
{
	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	// Effekseerを使用する場合は必ず設定する。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	// ただし、DirectX11を使用する場合は実行する必要はない。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effkseer_Init(8000) == -1)
	{
		return -1;
	}

	LoadEffectFile("explosion3");
	LoadEffectFile("hit");
	LoadEffectFile("gameClear");

	return 0;
}

void ThreeDimensionEffectManager::Update()
{
	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();
}

void ThreeDimensionEffectManager::Draw()
{
	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();
	DrawGraph(0, 0, imgHandle_, true);
}

void ThreeDimensionEffectManager::End()
{
	// Effekseerを終了する。
	Effkseer_End();
}

int ThreeDimensionEffectManager::LoadEffectFile(const char* fileName)
{
	std::string path = "Data/Effect/";
	path += fileName;
	path += ".efkefc";
	int handle = LoadEffekseerEffect(path.c_str());
	assert(handle != -1);
	effectResourceNameAndHandleTable_[fileName] = handle;
	return handle;
}

void ThreeDimensionEffectManager::PlayEffect(const char* fileName, VECTOR pos, float scale, float speed, VECTOR rot)
{
//	assert(effectResourceNameAndHandleTable_[fileName] != -1);
	effectNameAndHandleTable_[fileName] = PlayEffekseer3DEffect(effectResourceNameAndHandleTable_[fileName]);
	SetSpeedPlayingEffekseer3DEffect(effectNameAndHandleTable_[fileName], speed);
	SetScalePlayingEffekseer3DEffect(effectNameAndHandleTable_[fileName], scale, scale,scale);
	SetRotationPlayingEffekseer3DEffect(effectNameAndHandleTable_[fileName], rot.x, rot.y, rot.z);
	SetPosPlayingEffekseer3DEffect(effectNameAndHandleTable_[fileName], pos.x, pos.y, pos.z);
}

bool ThreeDimensionEffectManager::IsEffectPlaying(const char* fileName)
{
	if (IsEffekseer3DEffectPlaying(effectNameAndHandleTable_[fileName]) == 0)
	{
		return true;
	}
	return false;
}