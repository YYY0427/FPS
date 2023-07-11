#pragma once
#include <memory>
#include "../Model.h"
#include "../StageBase.h"

class Stage1 : public StageBase
{
public:
	Stage1();
	virtual ~Stage1();

	void Init();
	void Update();
	void Draw();
};