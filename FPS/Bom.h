#pragma once
#include <memory>
#include <DxLib.h>

class Model;

class Bom
{
public:
	Bom(VECTOR pos, VECTOR vec, float angle);
	virtual ~Bom();

	void Update();
	void Draw();

	float GetCollisionRadius() const { return collisionRadius_; }
	bool GetIsEnabled() const { return isEnabled_; }
	VECTOR GetPos() const { return pos_; }
	void SetIsExplosion();

private:
	std::shared_ptr<Model> pModel_;
	VECTOR pos_;
	VECTOR vec_;
	float collisionRadius_;
	bool isExplosion_;
	bool isEnabled_;
	int frameCount_;
	int effectH_;
	int effectResourceHandle_;
	int grFrontHandle_;
};