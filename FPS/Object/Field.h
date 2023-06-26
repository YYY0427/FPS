#pragma once
#include <memory>
#include "../Model.h"

class Field
{
public:
	Field(const char* fileName, float scale);
	virtual ~Field();

	void Init();
	void Update();
	void Draw();

	int GetModelHandle() const { return pModel_->GetModelHandle(); }
private:
	std::shared_ptr<Model> pModel_;
	const char* fileName_;
	float scale_;
};