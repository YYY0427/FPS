#pragma once

namespace Game
{
#ifdef _DEBUG
	// Window���[�h�ݒ�
	constexpr bool window_mode = true;
#else
	constexpr bool window_mode = false;
#endif

	// Window��
	const char* const title_text = "FPS";

	// Window�T�C�Y
	constexpr int screen_width = 1920;
	constexpr int screen_height = 1080;

	// �J���[���[�h
	constexpr int color_depth = 32;		// 32 or 16
};