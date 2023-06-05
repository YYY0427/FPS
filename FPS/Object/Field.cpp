#include "Field.h"
#include "../Model.h"
#include <DxLib.h>

namespace
{
	// ���f���̃t�@�C����
	const char* const file_name = "Data/Model/field.mv1";

	// �n�ʂɕ~���l�߂�u���b�N�̐�
	constexpr int block_num_x = 20;
	constexpr int block_num_z = 20;

	constexpr int block_num = block_num_x * block_num_z;

	// �u���b�N��1�ӂ̒���
	constexpr float block_side_length = 200.0f;

	// �n�ʂ�1�ӂ̒���
	constexpr float field_side_length_x = block_side_length * block_num_x;
	constexpr float field_side_length_z = block_side_length * block_num_z;
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
	m_pModel.push_back(std::make_shared<Model>(file_name));

	// �ŏ��Ƀ��[�h�������f���ƍ��킹�ă��f����100����
	int orgModel = m_pModel[0]->GetModelHandle();
	for (int i = 0; i < block_num - 1; i++)
	{
		// �ŏ��Ƀ��[�h�������f�����R�s�[
		m_pModel.push_back(std::make_shared<Model>(orgModel));
	}

	// �n�ʂɕ��ׂ�
	for (int i = 0; i < m_pModel.size(); i++)
	{
		float x = block_side_length * (i % block_num_x) - field_side_length_x / 2 + block_side_length / 2;
		float z = block_side_length * (i / block_num_z) - field_side_length_z / 2 + block_side_length / 2;
		m_pModel[i]->SetPos(VGet(x, -block_side_length / 2.0f, z));		// ��� y = 0.0f�ɂȂ�悤�z�u
	}
}

void Field::Update()
{
}

void Field::Draw()
{
	for (auto& model : m_pModel)
	{
		model->Draw();
	}
}