#include "FieldManager.h"
#include "Field.h"

namespace
{
	// ƒAƒhƒŒƒX
	const char* const field_adress = "Data/Model/bridge.mv1";
}

FieldManager::FieldManager()
{
	pField_.push_back(std::make_shared<Field>(field_adress, 50.0f));
}

FieldManager::~FieldManager()
{
}

void FieldManager::Init()
{
	for (auto& field : pField_)
	{
		field->Init();
	}
}

void FieldManager::Update()
{
	for (auto& field : pField_)
	{
		field->Update();
	}
}

void FieldManager::Draw()
{
	for (auto& field : pField_)
	{
		field->Draw();
	}
}