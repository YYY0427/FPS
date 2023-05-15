#include <DxLib.h>
#include <cassert>
#include "DrawFunctions.h"

namespace my
{
	/// <summary>
	/// 画像をロードする
	/// </summary>
	/// <param name="path">パス文字列</param>
	/// <returns>画像のハンドル</returns>

	int MyLoadGraph(const TCHAR* path)
	{
		int handle = LoadGraph(path);
		assert(handle >= 0);
		return handle;
	}
	int MyDrawRectRotaGraph(int x, int y, int left, int top, int width, int height, float scale, float angle, int handle, bool transFlg)
	{
		return DrawRectRotaGraph(x, y,
			left, top,
			width, height,
			scale, angle,
			handle, transFlg);
	}
	int MyDrawRectRotaGraph(float x, float y, int left, int top, int width, int height, float scale, float angle, int handle, bool transFlg)
	{
		return DrawRectRotaGraph(static_cast<int>(x), static_cast<int>(y),
			left, top,
			width, height,
			scale, angle,
			handle, transFlg);
	}
}


