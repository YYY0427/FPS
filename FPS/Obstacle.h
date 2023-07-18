#pragma once
#include <memory>
#include <DxLib.h>

class Model;
class UI;

class Obstacle
{
public:
	Obstacle(VECTOR pos, VECTOR rot);
	virtual ~Obstacle();

	void Update();
	void Draw();
	void DrawUI();
	void OnDamage(int damage);
	VECTOR GetPos() const { return VGet(pos_.x, pos_.y + 150.0f, pos_.z); }
	float GetNormalCollsionRadius() const;
	float GetTowerToCollsionRadius() const;
	bool GetIsEnabled() const { return isEnabled_; }
	int GetModelHandle() const;
private:
	VECTOR pos_;
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<UI> pUI_;
	int hp_;
	int damageFrame_;
	bool isEnabled_;
};