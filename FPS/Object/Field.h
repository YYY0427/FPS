#pragma once
#include <memory>

class Model;
class Field;

class Field
{
public:
	Field();
	virtual ~Field();

	void Init();
	void Update();
	void Draw();
private:
	std::shared_ptr<Model> pModel_;
};