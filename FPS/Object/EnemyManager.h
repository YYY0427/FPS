#pragma once
#include <memory>
#include <vector>

class EnemyBase;
class Player;
class Tower;
class EnemyShotFactory;
class Collision;

class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();

	void Create(int checkPoint, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory);
	void Update();
	void Draw();

	void DeadCheck();

	std::vector<std::shared_ptr<EnemyBase>> GetEnemies() const { return pEnemies_; }

private:
	std::vector<std::shared_ptr<EnemyBase>> pEnemies_;
};