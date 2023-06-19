#include "SkyDoom.h"
#include "Player.h"
#include "../Model.h"
#include <cassert>

namespace
{
	// �A�h���X
	const char* const file_name = "Data/Model/skyDoom.mv1";

	// ���f���̊g�嗦
	constexpr float model_magnification = 100.0f;
}

SkyDoom::SkyDoom(): 
	playerPos_(VGet(0, 0, 0))
{
}

SkyDoom::~SkyDoom()
{
}

void SkyDoom::Init()
{
	// �C���X�^���X��
	pModel_ = std::make_shared<Model>(file_name);

	// �v���C���[�̈ʒu�̎擾
	playerPos_ = pPlayer_->GetPos();

	// ���f���̃|�W�V�����̐ݒ�
	pModel_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));

	// ���f���̊g�嗦�̐ݒ�
	pModel_->SetScale(VGet(model_magnification, model_magnification, model_magnification));
}

void SkyDoom::Update()
{
	// �v���C���[�̈ʒu�̎擾
	playerPos_ = pPlayer_->GetPos();

	// ���f���̃|�W�V�����̐ݒ�
	pModel_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
}

void SkyDoom::Draw()
{
	// ���f���̕\��
	pModel_->Draw();
}
