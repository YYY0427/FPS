#include "EnemyShot.h"
#include "Model.h"
#include "Object/Player.h"

namespace
{
	// Ç±ÇÍà»è„ó£ÇÍÇΩÇÁè¡Ç∑
	constexpr float erase_distance = 18400.0f;

	// ÉÇÉfÉãÇÃägëÂó¶
	constexpr float model_scale = 0.5f;
}

EnemyShot::EnemyShot(int handle, std::shared_ptr<Player> pPlayer) : 
	pos_(VGet(0, 0, 0)),
	vec_(VGet(0, 0, 0)),
	isEnabled_(false), 
	handle_(handle),
	pPlayer_(pPlayer)
{
	Init();
}

EnemyShot::~EnemyShot()
{
}

void EnemyShot::Init()
{
	pModel_ = std::make_shared<Model>(handle_);
	pModel_->SetScale(VGet(model_scale, model_scale, model_scale));
}

void EnemyShot::Update()
{
	pos_ = VAdd(pos_, vec_);

	VECTOR toPlayer = VSub(pPlayer_->GetPos(), pos_);

	if (VSize(toPlayer) > erase_distance)
	{
		isEnabled_ = false;
	}

	pModel_->SetPos(pos_);
}

void EnemyShot::Draw()
{
	pModel_->Draw();
}

void EnemyShot::Start(const VECTOR& pos, const VECTOR& vec)
{
	pos_ = pos;
	vec_ = vec;
	isEnabled_ = true;
}