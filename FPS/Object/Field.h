#pragma once
#include <vector>
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
	std::vector<std::shared_ptr<Model>> m_pModel;
};