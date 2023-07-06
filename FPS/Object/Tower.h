#pragma once
#include <memory>
#include <DxLib.h>
#include "../Model.h"

class Model;

class Tower
{
public:
	Tower();
	virtual ~Tower();

	void Init();
	void Update();
	void Draw();

	void OnDamage(int damage);

	int GetModelHandle() const { return pModel_->GetModelHandle(); }
	VECTOR GetPos() const { return pos_; }
	float GetColRadius() const { return colRadius_; }
	int GetIsDead() const { return isDead_; }
private:
	std::shared_ptr<Model> pModel_;
	VECTOR pos_;
	int hp_;
	int damageFrame_;
	float colRadius_;
	bool isDead_;
};