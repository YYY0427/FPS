#include "BomManager.h"
#include "Bom.h"

BomManager::BomManager()
{
}

BomManager::~BomManager()
{
}



void BomManager::Update()
{
	for (auto& boms : pBoms_)
	{
		boms->Update();
	}

	auto boms = std::remove_if(pBoms_.begin(), pBoms_.end(), [](const std::shared_ptr<Bom>& boms)
		{
			return !boms->GetIsEnabled();
		});
	pBoms_.erase(boms, pBoms_.end());
}

void BomManager::Draw()
{
	for (auto& boms : pBoms_)
	{
		boms->Draw();
	}
}

void BomManager::StartBom(VECTOR pos, VECTOR vec, float angle)
{
	pBoms_.push_back(std::make_shared<Bom>(pos, vec, angle));
}
