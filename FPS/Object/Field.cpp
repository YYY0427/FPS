#include "Field.h"
#include "../Model.h"
#include <DxLib.h>

namespace
{
	// モデルのファイル名
	const char* const file_name = "Data/Model/bridge.mv1";

	// モデルの拡大率
	constexpr float model_magnification = 50.0f;
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
	pModel_ = std::make_shared<Model>(file_name);

	MV1SetScale(pModel_->GetModelHandle(), VGet(model_magnification, model_magnification, model_magnification));

	pModel_->SetPos(VGet(0, 0, 0));
}

void Field::Update()
{
}

void Field::Draw()
{
	pModel_->Draw();
}