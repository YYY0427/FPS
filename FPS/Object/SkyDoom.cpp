#include "SkyDoom.h"
#include <cassert>
namespace
{
	// アドレス
//	const char* const file_name = "Data/Model/SkyDoom.mv1";
	const char* const file_name = "Data/Model/realSkyDoom.mv1";

	// モデルの拡大率
	constexpr float model_rate = 0.99f;
}

SkyDoom::SkyDoom(): 
	handle_(-1)
{
}

SkyDoom::~SkyDoom()
{
	MV1DeleteModel(handle_);
}

void SkyDoom::Init()
{
	handle_ = MV1LoadModel(file_name);
	assert(handle_ != -1);

	MV1SetPosition(handle_, VGet(0, 0, 0));
	MV1SetScale(handle_, VGet(model_rate, model_rate, model_rate));
}

void SkyDoom::Update()
{
}

void SkyDoom::Draw()
{
	MV1DrawModel(handle_);
}
