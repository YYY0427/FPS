#pragma once
#include <DxLib.h>
#include <memory>

class Player;

class UI
{
public:
	UI();
	virtual ~UI();

	void DrawEnemyHpBar(std::shared_ptr<Player> pPlayer, VECTOR pos, int handle, int hp, int maxHp, const char* frameName, float height);
	void DrawAllyHpBar(int hp, int maxHp, int posX, int posY);

private:

	// HP�o�[�̉���
	int hpBarWidth_;

	// HP�o�[�̏c��
	int hpBarHeight_;

	int hpFontHandle_;
	int hpShadowFontHandle_;
};