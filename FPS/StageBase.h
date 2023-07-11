#pragma once
#include <memory>
#include <DxLib.h>
#include "Model.h"

class StageBase
{
public:
	StageBase();
	virtual ~StageBase();

	void Init();
	void Update();
	void Draw();

	int GetModelHandle() const { return pModel_->GetModelHandle(); }

protected:
	std::shared_ptr<Model> pModel_;

	// ���f���n���h���̃t�@�C��
	const char* fileName_ = nullptr;

	// �X�e�[�W�̊g�嗦
	float stageScale_ = 0.0f;

	// �X�e�[�W�̏����ʒu
	VECTOR initPos_{ 0.0f, 0.0f, 0.0f };
};