#pragma once
#include <memory>
#include <DxLib.h>

class Player;

class SkyDoom
{
public:
	SkyDoom();
	virtual ~SkyDoom();

	void Init();
	void Update();
	void Draw();

	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

private:
	std::shared_ptr<Player> pPlayer_;
	int handle_;
	VECTOR playerPos_;
};