#include "UI.h"

namespace
{
	// HP�o�[�̕\��
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
	//	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xff0000, true);

		// ���݂�HP(��)
		DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x - hpBarWidth_ / 2 + barWidth, screenPos.y + hpBarHeight_, 0x00ff00, true);

		// �g��
		DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x + hpBarWidth_ / 2, screenPos.y + hpBarHeight_, 0xffffff, false);

#ifdef _DEBUG
		//	DrawSphere3D(pos_, col_radius, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), false);
#endif
	}
}
