#include "StageManager.h"
#include "Object/Stage1.h"

StageManager::StageManager() :
	stageNum_(0)
{
}

StageManager::~StageManager()
{
}

void  StageManager::StageSelect(int stageNum)
{
	stageNum_ = stageNum;
	switch (stageNum_)
	{
	case 0:
		pStages_ = std::make_shared<Stage1>();
		break;
	}
	
}

void StageManager::Init()
{
	pStages_->Init();
}

void StageManager::Update()
{
	pStages_->Update();
}

void StageManager::Draw()
{
	pStages_->Draw();
}
