#include "Field.h"
#include "../Model.h"
#include <DxLib.h>

namespace
{
}

Field::Field(const char* fileName, float scale)
{
	fileName_ = fileName;
	scale_ = scale;
}

Field::~Field()
{
}

void Field::Init()
{
	// 3D���f�������[�h����
	pModel_ = std::make_shared<Model>(fileName_);

	// ���f���̊g�嗦�̐ݒ�
	MV1SetScale(pModel_->GetModelHandle(), VGet(scale_, scale_, scale_));

	// ���f���̈ʒu�̐ݒ�
	pModel_->SetPos(VGet(0, 0, 0));
}

void Field::Update()
{
}

void Field::Draw()
{
	// ���f���̕`��
	pModel_->Draw();
}