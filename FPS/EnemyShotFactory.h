#pragma once
#include <list>
#include <memory>
#include <DxLib.h>

class EnemyShot;
class Player;
class Tower;

class EnemyShotFactory
{
public:
	EnemyShotFactory(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower);
	virtual ~EnemyShotFactory();

	void Update();
	void Draw();

	void ShootStart(const VECTOR& pos, const VECTOR& vec, int target);

	std::list<std::shared_ptr<EnemyShot>> GetBullets() const { return pBullets_; }

private:
	int handle_;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Tower> pTower_;
	std::list<std::shared_ptr<EnemyShot>> pBullets_;
};