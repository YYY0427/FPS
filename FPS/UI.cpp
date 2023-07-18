#include "UI.h"

namespace
{
	// HPバーの表示
	constexpr int hp_bar_width = 64;
	constexpr int hp_bar_height = 10;
}

UI::UI()
{
	hpBarWidth_ = hp_bar_width;
	hpBarHeight_ = hp_bar_height;
}

UI::~UI()
{
}

void UI::DrawHpUI(int handle, int hp, int maxHp, const char* frameName, float height)
{
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	VECTOR startPos = ConvScreenPosToWorldPos(VGet(static_cast<float>(mouseX), static_cast<float>(mouseY), 0.0f));
	VECTOR endPos = ConvScreenPosToWorldPos(VGet(static_cast<float>(mouseX), static_cast<float>(mouseY), 1.0f));

	MV1SetupCollInfo(handle, -1, 8, 8, 8);

	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(handle, -1, startPos, endPos);
	if (hitPoly.HitFlag)
	{
		// モデル内にあるHPバーを表示する座標のデータを取得する
		int frameNo = MV1SearchFrame(handle, frameName);

		// HPバーを表示する座標データのワールド座標を取得する
		VECTOR hpPos = MV1GetFramePosition(handle, frameNo);

		hpPos.y += height;

		// HPバー表示位置のワールド座標をスクリーン座標に変換する
		VECTOR screenPos = ConvWorldPosToScreenPos(hpPos);

		if ((screenPos.z <= 0.0f) || (screenPos.z >= 1.0f))
		{
			// 表示範囲外の場合何も表示しない
			return;
		}

		// 最大HPに対する現在のHPの割合を計算する
		float hpRate = static_cast<float>(hp) / static_cast<float>(maxHp);

		// HPバーの長さを計算する
		float barWidth = hpBarWidth_ * hpRate;

		// HPバーの土台(赤)
	//	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xff0000, true);

		// 現在のHP(緑)
		DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x - hpBarWidth_ / 2 + barWidth, screenPos.y + hpBarHeight_, 0x00ff00, true);

		// 枠線
		DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x + hpBarWidth_ / 2, screenPos.y + hpBarHeight_, 0xffffff, false);

#ifdef _DEBUG
		//	DrawSphere3D(pos_, col_radius, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), false);
#endif
	}
}
