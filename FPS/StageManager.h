#pragma once
#include <vector>
#include <memory>

class StageBase;

class StageManager
{
public:
	StageManager();
	virtual ~StageManager();

	void StageSelect(int stageNum);
	void Init();
	void Update();
	void Draw();

	void SetStage(int stageNum) { stageNum_ = stageNum; }

	int GetAnyStage() const { return stageNum_; }
	std::shared_ptr<StageBase> GetStages() const { return pStages_; }
private:
	std::shared_ptr<StageBase> pStages_;
	int stageNum_;
};