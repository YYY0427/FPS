#pragma once
#include <DxLib.h>

class SkyDoom
{
public:
	SkyDoom();
	virtual ~SkyDoom();

	void Init();
	void Update();
	void Draw();

private:
	int handle_;
	VECTOR pos_;
};