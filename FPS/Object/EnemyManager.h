#pragma once
#include <memory>
#include <vector>

class EnemyBase;
class MainScene;

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

	std::vector<std::shared_ptr<EnemyBase>> GetEnemies() const { return pEnemies_; }

private:
	std::vector<std::shared_ptr<EnemyBase>> pEnemies_;
	std::shared_ptr<MainScene> pMainScene_;
};