#include "ObstacleManager.h"
#include "Obstacle.h"

namespace
{
	constexpr VECTOR obstacle_pos_1{ 1747.0f, -213.0f, -1405.0f };
	constexpr VECTOR obstacle_rot_1{ 0.0f, 45.0f * (DX_PI_F / 180.0f), 0.0f };

	constexpr VECTOR obstacle_pos_2{ -375.0f, -331.0f, -2250.0f };
	constexpr VECTOR obstacle_rot_2{ 0.0f, -20.0f * (DX_PI_F / 180.0f), 0.0f };

	constexpr VECTOR obstacle_pos_3{ -2075.0f, -297.0f, -3500.0f };
	constexpr VECTOR obstacle_rot_3{ 0.0f, -40.0f * (DX_PI_F / 180.0f), 0.0f };
}

ObstacleManager::ObstacleManager()
{
	pObstacles_.push_back(std::make_shared<Obstacle>(obstacle_pos_1, obstacle_rot_1));
	pObstacles_.push_back(std::make_shared<Obstacle>(obstacle_pos_2, obstacle_rot_2));
	pObstacles_.push_back(std::make_shared<Obstacle>(obstacle_pos_3, obstacle_rot_3));
}

ObstacleManager::~ObstacleManager()
{
}

void ObstacleManager::Update()
{
	for (auto& obj : pObstacles_)
	{
		obj->Update();
	}

	auto obj = std::remove_if(pObstacles_.begin(), pObstacles_.end(), [](const std::shared_ptr<Obstacle>& obj)
		{
			return !obj->GetIsEnabled();
		});
	pObstacles_.erase(obj, pObstacles_.end());
}

void ObstacleManager::Draw()
{
	for (auto& obj : pObstacles_)
	{
		obj->Draw();
	}
}
