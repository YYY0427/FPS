#pragma once
#include <memory>
#include <vector>

class Enemy;
class Player;

class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();

	void Init();
	void Update();
	void Draw();
	void DrawUI();

	void DeadCheck();

	std::vector<std::shared_ptr<Enemy>> GetEnemies() const { return pEnemies_; }
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

private:
	std::vector<std::shared_ptr<Enemy>> pEnemies_;
	std::shared_ptr<Player> pPlayer_;
};