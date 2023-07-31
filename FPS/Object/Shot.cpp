#include "Shot.h"
#include "Player.h"
#include "../Camera.h"
#include "../Model.h"
#include <cassert>

namespace
{
	// ����ȏ㗣�ꂽ�����
	constexpr float erase_distance = 18400.0f;

	// ���f���̃A�h���X
	const char* const bullet_file_name = "Data/Model/bullet.mv1";

	// ���f���̊g�嗦
	constexpr float model_magnification = 25.0f;

	// �����蔼�a�̃T�C�Y
	constexpr float collision_radius = 36.0f;
}

Shot::Shot() :
	handle_(-1),
	isEnabled_(false),
	pos_(VGet(0, 0, 0)),
	lastPos_(VGet(0, 0, 0)),
	vec_(VGet(0, 0, 0))
{
	// ���f���𕡐������Ƀ��f���{�̂��C���X�^���X��
	pModel_ = std::make_shared<Model>(bullet_file_name);

	// ���f���̊g�嗦�̐ݒ�
	pModel_->SetScale(VGet(model_magnification, model_magnification, model_magnification));

	pModel_->SetUseCollision(true);
}

Shot::Shot(int handle) :
	handle_(-1),
	isEnabled_(false),
	pos_(VGet(0, 0, 0)),
	lastPos_(VGet(0, 0, 0)),
	vec_(VGet(0, 0, 0))
{
	// ���f���{�̂𕡐����ăC���X�^���X��
	pModel_ = std::make_shared<Model>(handle);

	// ���f���̊g�嗦�̐ݒ�
	pModel_->SetScale(VGet(model_magnification, model_magnification, model_magnification));
}

Shot::~Shot()
{
}

void Shot::Update()
{
	if (!isEnabled_)	return;

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
		isEnabled_ = false;
	}

	// ���f���̃|�W�V�����̐ݒ�
	pModel_->SetPos(pos_);

	// FIXME:
	// �J�����̉�]
	pModel_->SetRot(VGet(pCamera_->GetCameraPitch() + DX_PI_F / 5, pCamera_->GetCameraYaw() + DX_PI_F, 0.0f));

	pModel_->Update();
}

void Shot::Draw()
{
	if (!isEnabled_)	return;
	pModel_->Draw();

#ifdef _DEBUG
//	DrawCapsule3D(pos_, lastPos_, col_radius, 8, 0xffffff, 0xff0000, false);
#endif 
}

int Shot::LoadModel() const
{
	// ���f���̃��[�h
	// 1�񃍁[�h(DuplicateModel���g���ăR�s�[���邽�߂ɕK�v)
	return MV1LoadModel(bullet_file_name);
}

void Shot::Start(VECTOR pos, VECTOR vec, std::shared_ptr<Player> pPlayer, std::shared_ptr<Camera> pCamera)
{
	// �V���b�g�J�n
	isEnabled_ = true;
	pos_ = pos;
	lastPos_ = pos;
	vec_ = vec;

	pPlayer_ = pPlayer;
	pCamera_ = pCamera;

	// ���f���̕������v���C���[�������Ă�������ɐݒ�
	pModel_->SetRot(VGet(pCamera_->GetCameraPitch(), pCamera_->GetCameraYaw(), 0.0f));

	// FIXME:
	// �J�����̉�]
	pModel_->SetRot(VGet(pCamera_->GetCameraPitch() + DX_PI_F / 5, pCamera_->GetCameraYaw() + DX_PI_F, 0.0f));
}

float Shot::GetColRadius() const
{
	return collision_radius;
}

int Shot::GetModelHandle() const
{
	return pModel_->GetModelHandle();
}
