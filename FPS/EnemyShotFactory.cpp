#include "EnemyShotFactory.h"
#include "EnemyShot.h"
#include <cassert>

namespace
{
	const char* const shot_adress = "Data/Model/sord.mv1";
}

EnemyShotFactory::EnemyShotFactory(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower) :
	pPlayer_(pPlayer),
	pTower_(pTower)
{
	handle_ = MV1LoadModel(shot_adress);
	assert(handle_ != -1);
}

EnemyShotFactory::~EnemyShotFactory()
{
}

void EnemyShotFactory::Update()
{
	// ïsóvÇ…Ç»Ç¡ÇΩíeÇÃçÌèú
	auto rmIt = std::remove_if(pBullets_.begin(), pBullets_.end(), [](const std::shared_ptr<EnemyShot>& bullet)
		{
			return !bullet->IsEnabled();
		});
	pBullets_.erase(rmIt, pBullets_.end());


	for (auto& bullet : pBullets_)
	{
		bullet->Update();
	}
}

void EnemyShotFactory::Draw()
{
	for (auto& bullet : pBullets_)
	{
		bullet->Draw();
	}
}

void EnemyShotFactory::ShootStart(const VECTOR& pos, const VECTOR& vec, int target)
{
	pBullets_.push_back(std::make_shared<EnemyShot>(handle_, pos, vec, pPlayer_, pTower_, target));
}
