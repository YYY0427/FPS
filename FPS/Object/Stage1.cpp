#include "Stage1.h"
#include "../Model.h"
#include <DxLib.h>

namespace
{
	// アドレス
	const char* const field_adress = "Data/Model/field/field2.mv1";

	// 拡大率
	constexpr float scale = 100.0f;

	// 初期位置
	constexpr VECTOR bee_init_pos_1{ 0.0f, -6000.0f, 0.0f };
}

Stage1::Stage1()
{
	fileName_ = field_adress;
	stageScale_ = scale;
	initPos_ = bee_init_pos_1;
}

Stage1::~Stage1()
{
}