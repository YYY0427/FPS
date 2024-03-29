#include "UI.h"
#include "Object/Player.h"

namespace
{
	// 敵のHPバー
	constexpr int enemy_hp_bar_width = 64;
	constexpr int enemy_hp_bar_height = 10;

	// プレイヤーとタワーのHPバー
	constexpr int ally_hp_bar_width = 400;
	constexpr int ally_hp_bar_height = 50;
}

UI::UI()
{
	hpFontHandle_ = CreateFontToHandle("ニコカv2", 20, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	hpShadowFontHandle_ = CreateFontToHandle("ニコカv2", 21, 3, DX_FONTTYPE_ANTIALIASING_4X4);
}

UI::~UI()
{
}

void UI::DrawEnemyHpBar(std::shared_ptr<Player> pPlayer, VECTOR pos, int handle, int hp, int maxHp, const char* frameName, float height)
{
	hpBarWidth_ = enemy_hp_bar_width;
	hpBarHeight_ = enemy_hp_bar_height;
	/*int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	VECTOR startPos = ConvScreenPosToWorldPos(VGet(static_cast<float>(mouseX), static_cast<float>(mouseY), 0.0f));
	VECTOR endPos = ConvScreenPosToWorldPos(VGet(static_cast<float>(mouseX), static_cast<float>(mouseY), 1.0f));

	MV1SetupCollInfo(handle, -1, 8, 8, 8);

	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(handle, -1, startPos, endPos);*/
//	if (hitPoly.HitFlag)
	float dist = VSize(VSub(pPlayer->GetPos(), pos));
	if (dist < (pPlayer->GetCollisionRadius() + 3000.0f))
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
	//	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0x000000, true);

		// 現在のHP(緑)
	//	DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x - hpBarWidth_ / 2 + barWidth, screenPos.y + hpBarHeight_, 0xff0000, true);

		DrawRoundRectAA(screenPos.x - enemy_hp_bar_width / 2, screenPos.y, screenPos.x + enemy_hp_bar_width / 2, screenPos.y + enemy_hp_bar_height, 8, 8, 4, 0x000000, true);

		if(hp > 0)
			DrawRoundRectAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x - hpBarWidth_ / 2 + barWidth, screenPos.y + hpBarHeight_, 8, 8, 4, 0xff0000, true);
		// 枠線
	//	DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x + hpBarWidth_ / 2, screenPos.y + hpBarHeight_, 0xffffff, false);
	}
}

void UI::DrawAllyHpBar(int hp, int maxHp, int posX, int posY)
{
	hpBarWidth_ = ally_hp_bar_width;
	hpBarHeight_ = ally_hp_bar_height;
		
	// 最大HPに対する現在のHPの割合を計算する
	float hpRate = static_cast<float>(hp) / static_cast<float>(maxHp);

	// HPバーの長さを計算する
	float barWidth = hpBarWidth_ * hpRate;

//	DrawRoundRectAA(posX - ally_hp_bar_width / 2, posY, posX + ally_hp_bar_width / 2, posY + ally_hp_bar_height, 1, 1, 4, 0x000000, true);
	DrawRoundRectAA(posX - ally_hp_bar_width / 2, posY, posX + ally_hp_bar_width / 2, posY + ally_hp_bar_height, 3, 3, 4, 0xffffff, false);

	if (hp > 0)
		DrawRoundRectAA(posX - hpBarWidth_ / 2, posY, posX - hpBarWidth_ / 2 + barWidth, posY + hpBarHeight_, 3, 3, 4, 0x22EA22, true);
		//DrawRoundRectAA(posX - hpBarWidth_ / 2, posY, posX - hpBarWidth_ / 2 + barWidth, posY + hpBarHeight_, 1, 1, 4, 0xffffff, true);

	DrawFormatStringToHandle(posX - 50, posY + 12, 0x000000, hpShadowFontHandle_, "%d", hp);
	DrawStringToHandle(posX + 8, posY + 12, "|", 0x000000, hpShadowFontHandle_);
	DrawFormatStringToHandle(posX + 25, posY + 12, 0x000000, hpShadowFontHandle_, "%d", maxHp);

	DrawFormatStringToHandle(posX - 50, posY + 12, 0xffffff, hpFontHandle_, "%d", hp);
	DrawStringToHandle(posX + 8, posY + 12, "|", 0xffffff, hpFontHandle_);
	DrawFormatStringToHandle(posX + 25, posY + 12, 0xffffff, hpFontHandle_, "%d", maxHp);
}
