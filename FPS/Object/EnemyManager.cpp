#include "EnemyManager.h"
#include "Enemy.h"
#include "Bee.h"
#include "EnemyBoss.h"
#include "Tower.h"
#include "../Scene/MainScene.h"
#include "Player.h"
#include "../Game.h"

namespace
{
	// èâä˙à íu
	constexpr VECTOR bee_init_pos_1{ 2931,  500, 3075};
	constexpr VECTOR bee_init_pos_2{ 3606,  500, -1710 };
	constexpr VECTOR bee_init_pos_3{ 5558,  1212, 1369 };
	constexpr VECTOR bee_init_pos_4{ 1342, 500, -1515 };
	constexpr VECTOR bee_init_pos_5{ 1710, 500, -1875 };
	constexpr VECTOR bee_init_pos_6{ -514, 600, -835 };
	constexpr VECTOR bee_init_pos_7{ -1642, 1000, -3045};
	constexpr VECTOR bee_init_pos_8{ -3701, 500, -606};
	constexpr VECTOR bee_init_pos_9{ -3588, 500, 564};

	constexpr VECTOR enemy_init_pos_1{ 4426, -49, 3169 };
	constexpr VECTOR enemy_init_pos_2{ 4634, -69, 3499 };
	constexpr VECTOR enemy_init_pos_3{ 5599, 31, 49 };
	constexpr VECTOR enemy_init_pos_4{ 5346, 6, 10 };
	constexpr VECTOR enemy_init_pos_5{ 2117, 500, -1017 };
	constexpr VECTOR enemy_init_pos_6{ 1918, 500, -781 };
	constexpr VECTOR enemy_init_pos_7{ 2286, 500, -1221 };
	constexpr VECTOR enemy_init_pos_8{ -2686, 266, 642 };
	constexpr VECTOR enemy_init_pos_9{ -2947, 268, 51 };
	constexpr VECTOR enemy_init_pos_10{ -3387, 213, -3901 };
	constexpr VECTOR enemy_init_pos_11{ -3727, 193, -3302 };
	constexpr VECTOR enemy_bos_init_pos{-3596, 221, -3577};

	// ãCãÖÇ™1Ç¬ÇﬂÇÃçÚÇÃãﬂÇ≠ÇÃèÍçá
	constexpr VECTOR addition_create_pos_1{ 4322, 188, -3282 };
	constexpr VECTOR addition_create_pos_2{ 1391, -251, 2771 };

	// ãCãÖÇ™2Ç¬ÇﬂÇÃçÚÇÃãﬂÇ≠ÇÃèÍçá
	constexpr VECTOR addition_create_pos_3{ -5841, 277, 193 };
	constexpr VECTOR addition_create_pos_4{ -4447, -153, -4550 };


	const char* const exclamation_mark_file_path = "Data/Model/biltukri.mv1";
}

EnemyManager::EnemyManager()
{
	handle_ = MV1LoadModel(exclamation_mark_file_path);
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::FirstCreate(int checkPoint, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, MainScene* pMainScene)
{
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_1, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_2, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_3, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_4, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_5, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_6, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_7, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_8, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_9, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_10, false, handle_));
	pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_init_pos_11, false, handle_));
	pEnemies_.push_back(std::make_shared<EnemyBoss>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, enemy_bos_init_pos, false, handle_));

	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, bee_init_pos_1, false, handle_));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, bee_init_pos_2, false, handle_));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, bee_init_pos_3, false, handle_));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, bee_init_pos_4, false, handle_));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, bee_init_pos_5, false, handle_));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, bee_init_pos_6, false, handle_));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, bee_init_pos_7, false, handle_));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, bee_init_pos_8, false, handle_));
	pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, bee_init_pos_9, false, handle_));
}

void EnemyManager::AdditionCreate(int checkPoint, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, MainScene* pMainScene)
{
	static int enemyTimer = 0;
	static int beeTimer = 0;
	static int enemyBossTimer = 0;
	VECTOR respornPos1 = VGet(0,0,0);
	VECTOR respornPos2 = VGet(0, 0, 0);
	if (checkPoint <= 4)
	{
		respornPos1 = addition_create_pos_1;
		respornPos2 = addition_create_pos_2;
	}
	else
	{
		respornPos1 = addition_create_pos_3;
		respornPos2 = addition_create_pos_4;
	}
	if (enemyTimer++ > 60 * 40)
	{
		for (int i = 0; i < 5; i++)
		{
			int num = GetRand(1) + 1;
			pEnemies_.push_back(std::make_shared<Enemy>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, (num == 1 ? respornPos1 : respornPos2), true, handle_));
		}
		enemyTimer = 0;
	}
	if (beeTimer++ > 60 * 40)
	{
		for (int i = 0; i < 5; i++)
		{
			respornPos1.y = 500;
			respornPos2.y = 500;
			int num = GetRand(1) + 1;
			pEnemies_.push_back(std::make_shared<Bee>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, (num == 1 ? respornPos1 : respornPos2), true, handle_));
		}
		beeTimer = 0;
	}
	if (enemyBossTimer++ > 60 * 40)
	{
		int num = GetRand(1) + 1;
		pEnemies_.push_back(std::make_shared<EnemyBoss>(pPlayer, pTower, pCollision, pEnemyShotFactory, pMainScene, (num == 1 ? respornPos1 : respornPos2), true, handle_));
		enemyBossTimer = 0;
	}

	for (auto& enemies : pEnemies_)
	{
		if (20000 < VSize(VSub(pPlayer->GetPos(), enemies->GetPos())))
		{
			enemies->SetIsDead(true);
		}
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

	/*for (int i = 0; i < pEnemies_.size(); i++)
	{
		VECTOR screenPos = ConvWorldPosToScreenPos(pEnemies_[i]->GetPos());

		DrawFormatString(screenPos.x, screenPos.y, 0x000000, "%d", i + 1);
	}*/
}

int EnemyManager::EnemyCount()
{
	int count = 0;
	for (auto& enemies : pEnemies_)
	{
		count++;
	}
	return count;
}

void EnemyManager::DeadCheck()
{
	auto enemies = std::remove_if(pEnemies_.begin(), pEnemies_.end(), [](const std::shared_ptr<EnemyBase>& enemies)
		{
			return enemies->GetDead();
		});
	pEnemies_.erase(enemies, pEnemies_.end());
}
