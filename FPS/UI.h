#pragma once
#include <DxLib.h>

class UI
{
public:
	UI();
	virtual ~UI();

	void DrawHpUI(int handle, int hp, int maxHp, const char* frameName, float height);

private:

	// HP�o�[�̉���
	int hpBarWidth_;

	// HP�o�[�̏c��
	int hpBarHeight_;
};