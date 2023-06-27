#pragma once
#include <memory>
#include <DxLib.h>

class Model;

class Tower
{
public:
	Tower();
	virtual ~Tower();

	void Init();
	void Update();
	void Draw();

	VECTOR GetPos() const { return pos_; }
private:
	std::shared_ptr<Model> pModel_;
	VECTOR pos_;
};