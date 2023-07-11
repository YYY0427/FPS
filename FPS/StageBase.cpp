#include "StageBase.h"
#include "Model.h"

StageBase::StageBase()
{
}

StageBase::~StageBase()
{
}

void StageBase::Init()
{
	// 3Dモデルをロードする
	pModel_ = std::make_shared<Model>(fileName_);

	// モデルの拡大率の設定
	MV1SetScale(pModel_->GetModelHandle(), VGet(stageScale_, stageScale_, stageScale_));

	// モデルの位置の設定
	pModel_->SetPos(initPos_);
}

void StageBase::Update()
{
}

void StageBase::Draw()
{
	// モデルの描画
	pModel_->Draw();
}
