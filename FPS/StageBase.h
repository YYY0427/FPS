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

	// モデルハンドルのファイル
	const char* fileName_ = nullptr;

	// ステージの拡大率
	float stageScale_ = 0.0f;

	// ステージの初期位置
	VECTOR initPos_{ 0.0f, 0.0f, 0.0f };
};