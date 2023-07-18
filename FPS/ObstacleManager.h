#pragma once
#include <vector>
#include <memory>
#include <DxLib.h>

class Obstacle;
class Tower;

class ObstacleManager
{
public:
	ObstacleManager(std::shared_ptr<Tower> pTower);
	virtual ~ObstacleManager();

	void Update();
	void Draw();
	void DrawUI();

	std::vector<std::shared_ptr<Obstacle>> GetObstacles() const { return pObstacles_; }
private:
	std::shared_ptr<Tower> pTower_;
	std::vector<std::shared_ptr<Obstacle>> pObstacles_;
};