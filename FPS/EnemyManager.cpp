#include "EnemyManager.h"
#include "Object/Enemy.h"

namespace
{
	// �G�̃A�h���X
	const char* const enemy_adress = "Data/Model/Enemy.mv1";
}

EnemyManager::EnemyManager()
{
	
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Init()
{
	pEnemies_.push_back(std::make_shared<Enemy>(enemy_adress, pPlayer_));
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
	auto enemies = std::remove_if(pEnemies_.begin(), pEnemies_.end(), [](const std::shared_ptr<Enemy>& enemies)
		{
			return enemies->GetDead();
		});
	pEnemies_.erase(enemies, pEnemies_.end());
}