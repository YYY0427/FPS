#pragma once
#include <DxLib.h>

class UI
{
public:
	UI();
	virtual ~UI();

	void DrawHpUI(int handle, int hp, int maxHp, const char* frameName, float height);

private:

	// HPバーの横幅
	int hpBarWidth_;

	// HPバーの縦幅
	int hpBarHeight_;
};