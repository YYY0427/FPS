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
	// 3D���f�������[�h����
	pModel_ = std::make_shared<Model>(fileName_);

	// ���f���̊g�嗦�̐ݒ�
	MV1SetScale(pModel_->GetModelHandle(), VGet(stageScale_, stageScale_, stageScale_));

	// ���f���̈ʒu�̐ݒ�
	pModel_->SetPos(initPos_);
}

void StageBase::Update()
{
}

void StageBase::Draw()
{
	// ���f���̕`��
	pModel_->Draw();
}
