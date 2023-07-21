#include "EnemyManager.h"
#include "Enemy.h"
#include "Bee.h"
#include "Tower.h"
#include "../Game.h"

namespace
{
	// èâä˙à íu
	constexpr VECTOR bee_init_pos_1{ 2931,  500, 3075};
	constexpr VECTOR bee_init_pos_2{ 3606,  500, -1710 };
	constexpr VECTOR bee_init_pos_3{ 5558,  1112, 1369 };

	constexpr VECTOR enemy_init_pos_1{ 4426, -49, 3169 };
	constexpr VECTOR enemy_init_pos_2{ 4234, -112, 2920 };
	constexpr VECTOR enemy_init_pos_3{ 4634, -69, 3499 };
	constexpr VECTOR enemy_init_pos_4{ 5599, 31, 49 };
	constexpr VECTOR enemy_init_pos_5{ 5346, 6, 10 };

	const char* const exclamation_mark_file_path = "Data/Model/biltukri.mv1";
}

EnemyManager::EnemyManager()
{
	handle_ = MV1LoadModel(exclamation_mark_file_path);
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Create(int checkPoint, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory)
{
	if (checkPoint == 1)
	{
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_1, false, handle_));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_2, false, handle_));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_3, false, handle_));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_4, false, handle_));
		pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, enemy_init_pos_5, false, handle_));

		pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_1, false, handle_));
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_2, false, handle_));
		pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, bee_init_pos_3, false, handle_));
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

	for (int i = 0; i < pEnemies_.size(); i++)
	{
		VECTOR screenPos = ConvWorldPosToScreenPos(pEnemies_[i]->GetPos());

		DrawFormatString(screenPos.x, screenPos.y, 0x000000, "%d", i + 1);
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
