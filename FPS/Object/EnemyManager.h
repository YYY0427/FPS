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
	EnemyManager(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory);
	virtual ~EnemyManager();

	void Create();
	void Update();
	void Draw();
	void DrawUI();

	void DeadCheck();

	std::vector<std::shared_ptr<EnemyBase>> GetEnemies() const { return pEnemies_; }

private:
	std::vector<std::shared_ptr<EnemyBase>> pEnemies_;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<EnemyShotFactory> pEnemyShotFactory_;
	std::shared_ptr<Collision> pCollision_;
};