#include "EnemyManager.h"
#include "Enemy.h"
#include "Bee.h"

namespace
{
	// “G‚ÌƒAƒhƒŒƒX
	const char* const enemy_adress = "Data/Model/enemyCol.mv1";
	const char* const bee_adress = "Data/Model/beeCol.mv1";
}

EnemyManager::EnemyManager(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory) :
	pPlayer_(pPlayer),
	pTower_(pTower),
	pCollision_(pCollision),
	pEnemyShotFactory_(pEnemyShotFactory)
{
	
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Create()
{
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_));
}

void EnemyManager::Update()
{
	for (auto& enemies : pEnemies_)
	{
		enemies->Update();
	}

	DeadCheck();
}

void EnemyManager::Draw()
{
	for (auto& enemies : pEnemies_)
	{
		enemies->Draw();
	}
}

void EnemyManager::DrawUI()
{
	for (auto& enemies : pEnemies_)
	{
		enemies->DrawUI();
	}
}

void EnemyManager::DeadCheck()
{
	auto enemies = std::remove_if(pEnemies_.begin(), pEnemies_.end(), [](const std::shared_ptr<EnemyBase>& enemies)
		{
			return enemies->GetDead();
		});
	pEnemies_.erase(enemies, pEnemies_.end());
}
