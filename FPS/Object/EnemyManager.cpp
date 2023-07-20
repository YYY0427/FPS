#include "EnemyManager.h"
#include "Enemy.h"
#include "Bee.h"
#include "Tower.h"

namespace
{
	// èâä˙à íu
	constexpr VECTOR bee_init_pos_1{ 2931,  500, 3075};
	constexpr VECTOR bee_init_pos_2{ 3606,  500, -1710 };
	constexpr VECTOR bee_init_pos_3{ 5558,  1112, 1369 };

	constexpr VECTOR enemy_init_pos_1{ 4426, -49, 3169 };
	constexpr VECTOR enemy_init_pos_2{ 4334, -143, 2752 };
	constexpr VECTOR enemy_init_pos_3{ 4841, -112, 3655 };
	constexpr VECTOR enemy_init_pos_4{ 5599, 31, 49 };
}

EnemyManager::EnemyManager()
{
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Create(int checkPoint, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory)
{
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_1, false));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_2, false));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_3, false));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_4, false));

	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_1, false));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_2, false));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_3, false));

	/*switch (checkPoint)
	{
	case Tower::CheckPoint::check_point1:
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_1, true));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_1, false));
		break;
	case Tower::CheckPoint::check_point2:
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_1, false));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_1, false));
		break;
	case Tower::CheckPoint::check_point3:
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_1, false));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_1, false));
		break;
	case Tower::CheckPoint::check_point4:
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_1, false));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_1, false));
		break;
	}*/
	
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
