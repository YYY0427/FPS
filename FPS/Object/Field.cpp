#include "Field.h"
#include "../Model.h"
#include <DxLib.h>

namespace
{
	// アドレス
	const char* const field_adress = "Data/Model/field/field2.mv1";

	// 拡大率
	constexpr float scale = 100.0f;

	// 初期位置
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
	// 3Dモデルをロードする
	pModel_ = std::make_shared<Model>(field_adress);

	// モデルの拡大率の設定
	MV1SetScale(pModel_->GetModelHandle(), VGet(scale, scale, scale));

	// モデルの位置の設定
	pModel_->SetPos(init_pos);
}

void Field::Update()
{

}

void Field::Draw()
{
	// モデルの描画
	pModel_->Draw();
}