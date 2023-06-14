#include "SkyDoom.h"

namespace
{
	// ƒAƒhƒŒƒX
	const char* const file_name = "Data/Model/SkyDoom.mv1";
}

SkyDoom::SkyDoom(): 
	handle_(-1),
	pos_(VGet(0, 0, 0))
{
}

SkyDoom::~SkyDoom()
{
	MV1DeleteModel(handle_);
}

void SkyDoom::Init()
{
	handle_ = MV1LoadModel(file_name);
	MV1SetPosition(handle_, pos_);
	MV1SetScale(handle_, VGet(50, 50, 50));
}

void SkyDoom::Update()
{
}

void SkyDoom::Draw()
{
	MV1DrawModel(handle_);
}
