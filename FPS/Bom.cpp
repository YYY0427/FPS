#include "Bom.h"
#include "Model.h"
#include "EffekseerForDXLib.h"

namespace
{
	const char* const file_path = "Data/Model/Bom.mv1";

	constexpr float befor_collision_radius = 80.0f;

	constexpr float after_collision_radius = 500.0f;

	constexpr float model_scale = 1.0f;

	constexpr int explosion_continuation_time = 30;
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
	else
	{
		frameCount_++;
		if (explosion_continuation_time > frameCount_)
		{
			isEnabled_ = false;
		}
	}
#ifdef _DEBUG
	DrawSphere3D(pos_, collisionRadius_, 16, 0xff0000, 0xff0000, false);
#endif
}

void Bom::StartExplosion()
{
	if (!isExplosion_)
	{
		isExplosion_ = true;
		collisionRadius_ = after_collision_radius;
		effectResourceHandle_ = LoadEffekseerEffect("Data/Effect/explosion.efkefc", 100.0f);
		grFrontHandle_ = LoadGraph("Data/Effect/png.png");
		effectH_ = PlayEffekseer3DEffect(effectResourceHandle_);
		SetSpeedPlayingEffekseer3DEffect(effectH_, 1.5f);
		SetPosPlayingEffekseer3DEffect(effectH_, pos_.x, pos_.y, pos_.z);
	}
	// 既に爆発していたら消す
	else
	{
		isEnabled_ = false;
	}
}