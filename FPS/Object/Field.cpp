#include "Field.h"
#include "../Model.h"
#include <DxLib.h>

namespace
{
	// �A�h���X
	const char* const field_adress = "Data/Model/field/field2.mv1";

	// �g�嗦
	constexpr float scale = 100.0f;

	// �����ʒu
	constexpr VECTOR init_pos{ 0.0f, -6000.0f, 0.0f };
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
	pModel_ = std::make_shared<Model>(field_adress);

	// ���f���̊g�嗦�̐ݒ�
	MV1SetScale(pModel_->GetModelHandle(), VGet(scale, scale, scale));

	// ���f���̈ʒu�̐ݒ�
	pModel_->SetPos(init_pos);
}

void Field::Update()
{

}

void Field::Draw()
{
	// ���f���̕`��
	pModel_->Draw();
}