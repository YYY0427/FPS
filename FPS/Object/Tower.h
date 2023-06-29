#pragma once
#include <memory>
#include <DxLib.h>
#include <DxDataTypeWin.h>

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

	VECTOR GetPos() const { return pos_; }
	float GetColRadius() const { return colRadius_; }
private:
	std::shared_ptr<Model> pModel_;
	VECTOR pos_;
	int hp_;
	int damageFrame_;
	float colRadius_;
};