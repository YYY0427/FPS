#include "EnemyShot.h"
#include "Model.h"
#include "Object/EnemyManager.h"
#include "Object/Tower.h"

namespace
{
	// �t�@�C���p�X
	const char* const shot_adress = "Data/Model/sord.mv1";

	// ���f���̊g�嗦
	constexpr float model_magnification = 25.0f;

	// �����蔼�a�̃T�C�Y
	constexpr float col_radius = 36.0f;

	// ����ȏ㗣�ꂽ�����
	constexpr float erase_distance = 18400.0f;
}

EnemyShot::EnemyShot()
{
}

EnemyShot::~EnemyShot()
{
}

void EnemyShot::Init(int handle)
{
	// �C���X�^���X��
	pModel_ = std::make_shared<Model>(handle);

	// ���f���̊g�嗦�̐ݒ�
	pModel_->SetScale(VGet(model_magnification, model_magnification, model_magnification));
}

void EnemyShot::Update()
{
	if (!isExsit_)	return;

	// �O�t���[���̈ʒu�̎擾
	lastPos_ = pos_;

	// �܂������i��
	pos_ = VAdd(pos_, vec_);

	// �v���C���[�̈ʒu�̎擾
	VECTOR towerPos = pTower_->GetPos();

	// �v���C���[����̋��������߂�
	VECTOR toPlayer = VSub(towerPos, pos_);

	// �v���C���[������ȏ㗣�ꂽ�����
	if (VSize(toPlayer) > erase_distance)
	{
		isExsit_ = false;
	}

	// ���f���̃|�W�V�����̐ݒ�
	pModel_->SetPos(pos_);

	// FIXME:
	// �J�����̉�]
//	pModel_->SetRot(VGet(pCamera_->GetCameraPitch() + DX_PI_F / 5, pCamera_->GetCameraYaw() + DX_PI_F, 0.0f));
}

void EnemyShot::Draw()
{
	if (!isExsit_)	return;
	pModel_->Draw();
}

int EnemyShot::LoadModel() const
{
	// ���f���̃��[�h
	// 1�񃍁[�h(DuplicateModel���g���ăR�s�[���邽�߂ɕK�v)
	return MV1LoadModel(shot_adress);
}

void EnemyShot::Start(VECTOR pos, VECTOR vec)
{
	// �V���b�g�J�n
	isExsit_ = true;
	pos_ = pos;
	lastPos_ = pos;
	vec_ = vec;

	// ���f���̕������v���C���[�������Ă�������ɐݒ�
//	pModel_->SetRot(VGet(pCamera_->GetCameraPitch(), pCamera_->GetCameraYaw(), 0.0f));

	// FIXME:
	// �J�����̉�]
//	pModel_->SetRot(VGet(pCamera_->GetCameraPitch() + DX_PI_F / 5, pCamera_->GetCameraYaw() + DX_PI_F, 0.0f));
}

float EnemyShot::GetColRadius() const
{
	return col_radius;
}
