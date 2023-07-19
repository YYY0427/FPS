#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>

class Obstacle;

class ObstacleManager
{
public:
	ObstacleManager();
	virtual ~ObstacleManager();

	void Update();
	void Draw();

	std::vector<std::shared_ptr<Obstacle>> GetObstacles() const { return pObstacles_; }
private:
	std::vector<std::shared_ptr<Obstacle>> pObstacles_;
};