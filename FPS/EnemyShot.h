#pragma once
#include <DxLib.h>
#include <memory>

class Model;
class Player;
class Tower;

class EnemyShot
{
public:
	EnemyShot(int handle, const VECTOR& pos, const VECTOR& vec, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, int target);
	virtual ~EnemyShot();

	void Update();
	void Draw();

	VECTOR GetPos() const { return pos_; }
	VECTOR GetLastPos() const { return lastPos_; }
	float GetColRadius() const;
	bool IsEnabled() const { return isEnabled_; }

	void SetIsEnabled(bool isEnabled) { isEnabled_ = isEnabled; }
private:
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Tower> pTower_;

	VECTOR pos_;
	VECTOR lastPos_;
	VECTOR initPos_;
	VECTOR vec_;
	VECTOR targetPos_;

	MATRIX scaleMatrix_;
	MATRIX rotateMatrix_;

	int target_;
	int handle_;
	bool isEnabled_;
};