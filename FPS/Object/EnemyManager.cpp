#include "EnemyManager.h"
#include "Enemy.h"
#include "Bee.h"
#include "Tower.h"

namespace
{
	// èâä˙à íu
	constexpr VECTOR bee_init_pos_1{ 5000.0f, 0.0f, 2200.0f };
	constexpr VECTOR enemy_init_pos_1{ 5000.0f, 100.0f, 2200.0f };
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

void EnemyManager::Create(int checkPoint)
{
	switch (checkPoint)
	{
	case Tower::CheckPoint::check_point1:
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, bee_init_pos_1));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, enemy_init_pos_1));
		break;
	case Tower::CheckPoint::check_point2:
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, bee_init_pos_1));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, enemy_init_pos_1));
		break;
	case Tower::CheckPoint::check_point3:
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, bee_init_pos_1));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, enemy_init_pos_1));
		break;
	case Tower::CheckPoint::check_point4:
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, bee_init_pos_1));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, enemy_init_pos_1));
		break;
	}
	
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

void EnemyManager::DeadCheck()
{
	auto enemies = std::remove_if(pEnemies_.begin(), pEnemies_.end(), [](const std::shared_ptr<EnemyBase>& enemies)
		{
			return enemies->GetDead();
		});
	pEnemies_.erase(enemies, pEnemies_.end());
}
