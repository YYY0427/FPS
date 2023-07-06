#include "SkyDoom.h"
#include "Player.h"
#include "../Model.h"
#include <cassert>

namespace
{
	// �A�h���X
	const char* const skydoom_file_name = "Data/Model/summerSkyDoom.mv1";
	const char* const cloud1_file_name = "Data/Model/cloud.mv1";
	const char* const cloud2_file_name = "Data/Model/cloud2.mv1";

	// ���f���̊g�嗦
	constexpr float model_magnification = 100.0f;
	constexpr float model_magnification2 = 13.0f;
}

SkyDoom::SkyDoom(): 
	playerPos_(VGet(0, 0, 0)),
	rot_(0.0f)
{
}

SkyDoom::~SkyDoom()
{
}

void SkyDoom::Init()
{
	// �C���X�^���X��
	pSkydoom_ = std::make_shared<Model>(skydoom_file_name);
	pCloud_ = std::make_shared<Model>(cloud1_file_name);
	pCloud2_ = std::make_shared<Model>(cloud2_file_name);

	// �v���C���[�̈ʒu�̎擾
	playerPos_ = pPlayer_->GetPos();

	// ���f���̃|�W�V�����̐ݒ�
	pSkydoom_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
	pCloud_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
	pCloud2_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));

	// ���f���̊g�嗦�̐ݒ�
	pSkydoom_->SetScale(VGet(model_magnification, model_magnification, model_magnification));
	pCloud_->SetScale(VGet(model_magnification2, model_magnification2, model_magnification2));
	pCloud2_->SetScale(VGet(model_magnification2, model_magnification2, model_magnification2));
}

void SkyDoom::Update()
{
	// ��]���Z
	rot_ += 0.0001f;

	// �v���C���[�̈ʒu�̎擾
	playerPos_ = pPlayer_->GetPos();

	// ���f���̃|�W�V�����̐ݒ�
	pSkydoom_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
	pCloud_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));
	pCloud2_->SetPos(VGet(playerPos_.x, 0, playerPos_.z));

	// ���f���̉�]�̐ݒ�
	pCloud_->SetRot(VGet(0, rot_, 0));
	pCloud2_->SetRot(VGet(0, rot_, 0));
}

void SkyDoom::Draw()
{
	MV1SetUseZBuffer(pSkydoom_->GetModelHandle(), false);

	// ���f���̕\��
	pSkydoom_->Draw();
	pCloud_->Draw();
	pCloud2_->Draw();
}
