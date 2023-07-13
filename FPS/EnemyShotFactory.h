#pragma once
#include <list>
#include <memory>
#include <DxLib.h>

class EnemyShot;
class Player;

class EnemyShotFactory
{
public:
	EnemyShotFactory(std::shared_ptr<Player> pPlayer);
	virtual ~EnemyShotFactory();

	void Init();
	void Update();
	void Draw();

	void ShootStart(const VECTOR& pos, const VECTOR& vec);

private:
	int handle_;
	std::shared_ptr<Player> pPlayer_;
	std::list<std::shared_ptr<EnemyShot>> pBullets_;
};