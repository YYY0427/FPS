#include "EnemyShot.h"
#include "Model.h"
#include "Object/Player.h"
#include "Object/Tower.h"
#include "Object/EnemyBase.h"

namespace
{
	// ����ȏ㗣�ꂽ�����
	constexpr float erase_distance = 18400.0f;

	// ���f���̊g�嗦
	constexpr float model_scale = 0.1f;

	// ���f���̓����蔻��̔��a
	constexpr float col_radius = 16.0f;

	constexpr VECTOR init_direction{ 0, 1, 0 };
}

EnemyShot::EnemyShot(int handle, const VECTOR& pos, const VECTOR& vec, std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, int target) :
	pos_(pos),
	vec_(vec),
	initPos_(pos),
	lastPos_(VGet(0, 0, 0)),
	isEnabled_(true), 
	handle_(handle),
	pPlayer_(pPlayer),
	pTower_(pTower),
	target_(target)
{
	switch (target_)
	{
	case EnemyBase::Target::player:
		targetPos_ = pPlayer_->GetPos();
		break;
	case EnemyBase::Target::tower:
		targetPos_ = pTower_->GetPos();
		break;
	}

	// ���f���̍쐬(�C���X�^���X��)
	pModel_ = std::make_shared<Model>(handle_);

	pModel_->SetUseCollision(true, true);

	// �^�[�Q�b�g�܂ł̃x�N�g���̍쐬
	VECTOR toTargetVec = VSub(targetPos_, pos_);

	// �g��s��̍쐬
	scaleMatrix_ = MGetScale(VGet(model_scale, model_scale, model_scale));

	// �^�[�Q�b�g�̕����̉�]�s��̍쐬
	rotateMatrix_ = MGetRotVec2(init_direction, toTargetVec);
}

EnemyShot::~EnemyShot()
{
}

void EnemyShot::Update()
{
	// �|�W�V�����̕ۑ�
	lastPos_ = pos_;

	// �ړ�
	pos_ = VAdd(pos_, vec_);

	// ���s�s��̍쐬
	MATRIX transMatrix = MGetTranslate(pos_);

	// �����ʒu���猻�݂̈ʒu�܂ł̃x�N�g���̍쐬
	VECTOR initPosToNowPosVec= VSub(initPos_, pos_);

	// �^�[�Q�b�g���炠����x�̋������ꂽ�����
	if (VSize(initPosToNowPosVec) > erase_distance)
	{
		isEnabled_ = false;
	}

	// �s��̍���
	MATRIX mat = MMult(scaleMatrix_, transMatrix);
	MATRIX mat2 = MMult(rotateMatrix_, mat);

	// ���f���̍��W�ϊ��p�s����Z�b�g����
	MV1SetMatrix(pModel_->GetModelHandle(), mat2);

	pModel_->Update();
}

void EnemyShot::Draw()
{
	DrawCapsule3D(pos_, lastPos_, col_radius, 16, 0xff0000, 0xff0000, false);
	pModel_->Draw();
}

float EnemyShot::GetColRadius() const
{
	return col_radius;
}
