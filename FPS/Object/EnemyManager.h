#pragma once
#include <memory>
#include <vector>

class EnemyBase;
class Player;
class Tower;
class EnemyShotFactory;
class Collision;
class MainScene;

class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();

	void FirstCreate(int checkPoint, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, MainScene* pMainScene);
	void AdditionCreate(int checkPoint, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, MainScene* pMainScene);

	void Update();
	void Draw();
	int EnemyCount();
	void DeadCheck();

	std::vector<std::shared_ptr<EnemyBase>> GetEnemies() const { return pEnemies_; }

private:
	std::vector<std::shared_ptr<EnemyBase>> pEnemies_;
	int handle_;
};