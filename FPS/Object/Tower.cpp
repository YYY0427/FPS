#include "Tower.h"
#include "../Model.h"

Tower::Tower() :
	pos_(VGet(0, 0, 0)),
	hp_(0)
{
}

Tower::~Tower()
{
}

void Tower::Init()
{
//	pModel_ = std::make_shared<Model>();
	pos_ = VGet(100, 100, 0);
}

void Tower::Update()
{

}

void Tower::Draw()
{
//	pModel_->Draw();

	DrawSphere3D(pos_, 100.0f, 50, 0xff0000, 0xff0000, true);
}