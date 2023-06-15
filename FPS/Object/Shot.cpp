#include "Shot.h"
#include "Player.h"
#include "../Camera.h"
#include "../Model.h"
#include <cassert>

namespace
{
	// ����ȏ㗣�ꂽ�����
	constexpr float erase_distance = 3000.0f;

	// ���f���̃A�h���X
	const char* const bullet_file_name = "Data/Model/bullet.mv1";

	// ���f���̊g�嗦
	constexpr float model_magnification = 25.0f;
}

Shot::Shot() :
	handle_(-1),
	isExsit_(false),
	pos_(VGet(0, 0, 0)),
	lastPos_(VGet(0, 0, 0)),
	vec_(VGet(0, 0, 0))
{
}

Shot::~Shot()
{
}

void Shot::Init(int handle)
{
	pModel_ = std::make_shared<Model>(handle);

	pModel_->SetScale(VGet(model_magnification, model_magnification, model_magnification));
//	pModel_->SetRot(VGet(0, pCamera_->GetCameraAngleX(), pCamera_->GetCameraAngleY()));
}

void Shot::Update()
{
	if (!isExsit_)	return;

	// �O�t���[���̈ʒu�̎擾
	lastPos_ = pos_;

	// �܂������i��
	pos_ = VAdd(pos_, vec_);

	// �v���C���[�̈ʒu�̎擾
	VECTOR playerPos = pPlayer_->GetPos();

	// �v���C���[����̋��������߂�
	VECTOR toPlayer = VSub(playerPos, pos_);

	// �v���C���[������ȏ㗣�ꂽ�����
	if (VSize(toPlayer) > erase_distance)
	{
		isExsit_ = false;
	}

	// ���f���̃|�W�V�����̐ݒ�
	pModel_->SetPos(pos_);
}

void Shot::Draw()
{
	if (!isExsit_)	return;
	pModel_->Draw();
}

int Shot::LoadModel() const
{
	// ���f���̃��[�h
	// 1�񃍁[�h(DuplicateModel���g���ăR�s�[���邽�߂ɕK�v)
	return MV1LoadModel(bullet_file_name);
}

void Shot::Start(VECTOR pos, VECTOR vec)
{
	// �V���b�g�J�n
	isExsit_ = true;
	pos_ = pos;
	lastPos_ = pos;
	vec_ = vec;

	// ���f���̕������v���C���[�������Ă�������ɐݒ�
	pModel_->SetRot(VGet(-pCamera_->GetCameraAngleY(), pCamera_->GetCameraAngleX(), 0));
}