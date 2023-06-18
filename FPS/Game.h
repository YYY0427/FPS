#pragma once

namespace Game
{
#ifdef _DEBUG
	// Windowモード設定
	constexpr bool window_mode = true;
#else
	constexpr bool window_mode = false;
#endif

	// Window名
	const char* const title_text = "FPS";

	// Windowサイズ
	constexpr int screen_width = 1920;
	constexpr int screen_height = 1080;

	// カラーモード
	constexpr int color_depth = 32;		// 32 or 16
};