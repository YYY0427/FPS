#pragma once
#include <DxLib.h>
#include <memory>

class Player;

class UI
{
public:
	UI();
	virtual ~UI();

	void DrawHpUI(std::shared_ptr<Player> pPlayer, VECTOR pos, int handle, int hp, int maxHp, const char* frameName, float height);

private:

	// HPバーの横幅
	int hpBarWidth_;

	// HPバーの縦幅
	int hpBarHeight_;
};