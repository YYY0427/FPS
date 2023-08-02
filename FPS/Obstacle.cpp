#include "Obstacle.h"
#include "Model.h"
#include "SoundManager.h"

namespace
{
	const char* const file_path = "Data/Model/stairs.mv1";

	constexpr float model_scale = 3.0f;

	constexpr float normal_collision_radius = 256.0f;
	constexpr float tower_collision_radius = 1026.0f;

	constexpr int max_hp = 150;

	constexpr int invincible_time = 10;
}

Obstacle::Obstacle(VECTOR pos, VECTOR rot)  :
	damageFrame_(0)
{
	pos_ = pos;
	sHp_.maxHp_ = max_hp;
	sHp_.hp_ = max_hp;
	sHp_.hpUIDrawY_ = 300.0f;
	isEnabled_ = true;
	pModel_ = std::make_shared<Model>(file_path);
	pModel_->SetUseCollision(true);
	pModel_->SetScale(VGet(model_scale, model_scale, model_scale));
	pModel_->SetPos(pos_);
	pModel_->SetRot(rot);
}

Obstacle::~Obstacle()
{

}

void Obstacle::Update()
{
	auto& soundManager = SoundManager::GetInstance();
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	if (sHp_.hp_ <= 0)
	{
		soundManager.Play3D("break", pos_, 10000.0f, false);
		isEnabled_ = false;
	}
	pModel_->Update();
}

void Obstacle::Draw()
{
	pModel_->Draw();

#ifdef _DEBUG
	DrawSphere3D(VGet(pos_.x, pos_.y + 150.0f, pos_.z), normal_collision_radius, 16.0f, 0xff0000, 0xff0000, false);
#endif
}

void Obstacle::OnDamage(int damage)
{
	// ƒ_ƒ[ƒWˆ—
	if (damageFrame_ > 0)	return;
	sHp_.hp_ -= damage;
	damageFrame_ = invincible_time;
}

float Obstacle::GetNormalCollsionRadius() const
{
	return normal_collision_radius;
}

float Obstacle::GetTowerToCollsionRadius() const
{
	return tower_collision_radius;
}

int Obstacle::GetModelHandle() const
{
	return pModel_->GetModelHandle();
}
