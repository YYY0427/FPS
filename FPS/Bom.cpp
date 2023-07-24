#include "Bom.h"
#include "Model.h"

namespace
{
	const char* const file_path = "Data/Model/Bom.mv1";

	constexpr float befor_collision_radius = 80.0f;

	constexpr float after_collision_radius = 500.0f;

	constexpr float model_scale = 1.0f;

	constexpr int explosion_continuation_time = 60;
}

Bom::Bom(VECTOR pos, VECTOR vec, float angle) :
	pos_(pos),
	vec_(vec),
	isExplosion_(false),
	isEnabled_(true),
	collisionRadius_(befor_collision_radius),
	frameCount_(0)
{
	pModel_ = std::make_shared<Model>(file_path);
	pModel_->SetPos(pos_);
	pModel_->SetScale(VGet(model_scale, model_scale, model_scale));
	pModel_->SetRot(VGet(0.0f, angle + DX_PI_F, 0.0f));
}

Bom::~Bom()
{
}

void Bom::Update()
{
	if (!isExplosion_)
	{
		// まっすぐ進む
		pos_ = VAdd(pos_, vec_);
	}
	else
	{
		collisionRadius_ = after_collision_radius;

		if (frameCount_++ > explosion_continuation_time)
		{
			isEnabled_ = false;
		}
	}

	// モデルのポジションの設定
	pModel_->SetPos(pos_);
}

void Bom::Draw()
{
	// 爆発していないときのみ描画
	if (!isExplosion_)
	{
		pModel_->Draw();
	}
#ifdef _DEBUG
	DrawSphere3D(pos_, collisionRadius_, 16.0f, 0xff0000, 0xff0000, false);
#endif
}

void Bom::SetIsExplosion()
{
	if (!isExplosion_)
	{
		isExplosion_ = true;
	}
	else
	{
		isEnabled_ = false;
	}
}