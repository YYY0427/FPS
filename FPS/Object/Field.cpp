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
	// 3Dモデルをロードする
	pModel_ = std::make_shared<Model>(fileName_);

	// モデルの拡大率の設定
	MV1SetScale(pModel_->GetModelHandle(), VGet(scale_, scale_, scale_));

	// モデルの位置の設定
	pModel_->SetPos(VGet(0, 0, 0));
}

void Field::Update()
{
}

void Field::Draw()
{
	// モデルの描画
	pModel_->Draw();
}