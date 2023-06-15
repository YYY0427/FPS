#include "Field.h"
#include "../Model.h"
#include <DxLib.h>

namespace
{
	// ���f���̃t�@�C����
	const char* const file_name = "Data/Model/Desert.mv1";

	// ���f���̊g�嗦
	constexpr float model_magnification = 25.0f;
}

Field::Field()
{
	
}

Field::~Field()
{
}

void Field::Init()
{
	// 3D���f�������[�h����
	pModel_ = std::make_shared<Model>(file_name);

	MV1SetScale(pModel_->GetModelHandle(), VGet(model_magnification, model_magnification, model_magnification));

	pModel_->SetPos(VGet(0, -80, 0));
}

void Field::Update()
{
}

void Field::Draw()
{
	pModel_->Draw();
}