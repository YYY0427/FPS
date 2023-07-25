#include "UI.h"
#include "Object/Player.h"

namespace
{
	// �G��HP�o�[
	constexpr int enemy_hp_bar_width = 64;
	constexpr int enemy_hp_bar_height = 10;

	// �v���C���[�ƃ^���[��HP�o�[
	constexpr int ally_hp_bar_width = 500;
	constexpr int ally_hp_bar_height = 30;
}

UI::UI()
{
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
	if (dist < (pPlayer->GetColRadius() + 3000.0f))
	{
		// ���f�����ɂ���HP�o�[��\��������W�̃f�[�^���擾����
		int frameNo = MV1SearchFrame(handle, frameName);

		// HP�o�[��\��������W�f�[�^�̃��[���h���W���擾����
		VECTOR hpPos = MV1GetFramePosition(handle, frameNo);

		hpPos.y += height;

		// HP�o�[�\���ʒu�̃��[���h���W���X�N���[�����W�ɕϊ�����
		VECTOR screenPos = ConvWorldPosToScreenPos(hpPos);

		if ((screenPos.z <= 0.0f) || (screenPos.z >= 1.0f))
		{
			// �\���͈͊O�̏ꍇ�����\�����Ȃ�
			return;
		}

		// �ő�HP�ɑ΂��錻�݂�HP�̊������v�Z����
		float hpRate = static_cast<float>(hp) / static_cast<float>(maxHp);

		// HP�o�[�̒������v�Z����
		float barWidth = hpBarWidth_ * hpRate;

		// HP�o�[�̓y��(��)
	//	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0x000000, true);

		// ���݂�HP(��)
	//	DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x - hpBarWidth_ / 2 + barWidth, screenPos.y + hpBarHeight_, 0xff0000, true);

		DrawRoundRectAA(screenPos.x - enemy_hp_bar_width / 2, screenPos.y, screenPos.x + enemy_hp_bar_width / 2, screenPos.y + enemy_hp_bar_height, 8, 8, 4, 0x000000, true);

		if(hp > 0)
			DrawRoundRectAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x - hpBarWidth_ / 2 + barWidth, screenPos.y + hpBarHeight_, 8, 8, 4, 0xff0000, true);
		// �g��
	//	DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x + hpBarWidth_ / 2, screenPos.y + hpBarHeight_, 0xffffff, false);
	}
}

void UI::DrawAllyHpBar(int hp, int maxHp, int posX, int posY)
{
	hpBarWidth_ = ally_hp_bar_width;
	hpBarHeight_ = ally_hp_bar_height;

	// �ő�HP�ɑ΂��錻�݂�HP�̊������v�Z����
	float hpRate = static_cast<float>(hp) / static_cast<float>(maxHp);

	// HP�o�[�̒������v�Z����
	float barWidth = hpBarWidth_ * hpRate;

	DrawRoundRectAA(posX - ally_hp_bar_width / 2, posY, posX + ally_hp_bar_width / 2, posY + ally_hp_bar_height, 16, 16, 4, 0x000000, true);

	if (hp > 0)
		DrawRoundRectAA(posX - hpBarWidth_ / 2, posY, posX - hpBarWidth_ / 2 + barWidth, posY + hpBarHeight_, 16, 16, 4, 0x00ff00, true);
	DrawRoundRectAA(posX - ally_hp_bar_width / 2, posY, posX + ally_hp_bar_width / 2, posY + ally_hp_bar_height, 16, 16, 4, 0x000000, false);

	DrawFormatString(posX + 200, posY - 20, 0x000000, "%d/%d", hp, maxHp);
	DrawFormatString(posX + 201, posY - 21, 0xffffff, "%d/%d", hp, maxHp);
}
