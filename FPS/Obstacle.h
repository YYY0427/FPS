#pragma once
#include <memory>
#include <DxLib.h>

class Model;

class Obstacle
{
public:
	Obstacle(VECTOR pos, VECTOR rot);
	virtual ~Obstacle();

	void Update();
	void Draw();
	void OnDamage(int damage);
	VECTOR GetPos() const { return VGet(pos_.x, pos_.y + 150.0f, pos_.z); }
	float GetNormalCollsionRadius() const;
	float GetTowerToCollsionRadius() const;
	bool GetIsEnabled() const { return isEnabled_; }
	int GetModelHandle() const;

private:
	struct HP
	{
		int hp_;
		int maxHp_;
		// HPを表示する際指定位置からどれだけY軸をプラスするか
		float hpUIDrawY_;
	};

public:
	HP GetHP() const { return sHp_; }

private:
	HP sHp_;
	VECTOR pos_;
	std::shared_ptr<Model> pModel_;
	int damageFrame_;
	bool isEnabled_;
};