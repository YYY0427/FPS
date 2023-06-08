#include "Field.h"
#include "../Model.h"
#include <DxLib.h>

namespace
{
	// モデルのファイル名
	const char* const file_name = "Data/Model/field.mv1";

	// 地面に敷き詰めるブロックの数
	constexpr int block_num_x = 20;
	constexpr int block_num_z = 20;

	constexpr int block_num = block_num_x * block_num_z;

	// ブロックの1辺の長さ
	constexpr float block_side_length = 200.0f;

	// 地面の1辺の長さ
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
	// 3Dモデルをロードする
	pModel_.push_back(std::make_shared<Model>(file_name));

	// 最初にロードしたモデルと合わせてモデルを100個生成
	int orgModel = pModel_[0]->GetModelHandle();
	for (int i = 0; i < block_num - 1; i++)
	{
		// 最初にロードしたモデルをコピー
		pModel_.push_back(std::make_shared<Model>(orgModel));
	}

	// 地面に並べる
	for (int i = 0; i < pModel_.size(); i++)
	{
		float x = block_side_length * (i % block_num_x) - field_side_length_x / 2 + block_side_length / 2;
		float z = block_side_length * (i / block_num_z) - field_side_length_z / 2 + block_side_length / 2;
		pModel_[i]->SetPos(VGet(x, -block_side_length / 2.0f, z));		// 上面 y = 0.0fになるよう配置
	}
}

void Field::Update()
{
}

void Field::Draw()
{
	for (auto& model : pModel_)
	{
		model->Draw();
	}
}