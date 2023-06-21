#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;

class SkyDoom
{
public:
	SkyDoom();
	virtual ~SkyDoom();

	void Init();
	void Update();
	void Draw();

	// プレイヤーポインタのセッター
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

private:
	// ポインタ
	std::shared_ptr<Model> pSkydoom_;
	std::shared_ptr<Model> pCloud_;
	std::shared_ptr<Model> pCloud2_;
	std::shared_ptr<Player> pPlayer_;

	// プレイヤーの位置
	VECTOR playerPos_;

	// 回転量
	float rot_;
};