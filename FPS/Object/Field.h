#pragma once
#include <memory>
#include "../Model.h"

class Field
{
public:
	Field();
	virtual ~Field();

	void Init();
	void Update();
	void Draw();

	int GetModelHandle() const { return pModel_->GetModelHandle(); }
private:
	std::shared_ptr<Model> pModel_;
};