#include "Player.h"
#include "../Model.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Scene/MainScene.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	// ���f���̃t�@�C����
	const char* const file_name = "data/model/player.mv1";

	// �v���C���[�̈ړ���
	constexpr VECTOR player_vec_z{ 0, 0, -10 };
	constexpr VECTOR player_vec_x{ -10, 0, 0 };
	constexpr VECTOR player_vec_y{ 0, -10, 0 };

	// �V���b�g�̑��x
	constexpr float shot_speed = 50.0f;

	// �W�����v��
	constexpr float jump_power = 16.0f;

	// �d��
	constexpr float gravity = -1.0f;

	// �A�j���[�V�����ԍ�
	constexpr int idle_anim_no = 3;			// �ҋ@���[�V����
	constexpr int idle_shot_anim_no = 5;	// ��~���Ă����ԂŃV���b�g������
	constexpr int walk_anim_no = 14;		// �ړ����[�V����	
	constexpr int punch_anim_no = 10;		// �e�ŉ��郂�[�V����
	constexpr int walk_shot_anim_no = 13;	// �ړ����Ă����ԂŃV���b�g������

	// �A�j���[�V�����؂�ւ��ɂ�����t���[����
	constexpr int anim_change_frame = 16;

	// �����蔼�a�̃T�C�Y
	constexpr float col_radius = 70.0f;

	// �ő�HP
	constexpr int max_hp = 5;

	// �_���[�W�H��������̖��G����
	constexpr int invincible_time = 60;
}

Player::Player() :
	updateFunc_(&Player::UpdateIdle),
	animNo_(idle_anim_no),
	frameCount_(0),
	pos_(VGet(0, 0, 0)),
	jumpAcc_(0.0f),
	hp_(max_hp),
	damageFrame_(0)
{
	// 3D���f���̐���
	pModel_ = std::make_shared<Model>(file_name);
	pModel_->SetAnimation(animNo_, true, true);
}

Player::~Player()
{
}

void Player::Init()
{
}

void Player::Update(const InputState& input)
{
	(this->*updateFunc_)(input);
}

void Player::Draw()
{
	// HP�̕`��
	for (int i = 0; i < hp_; i++)
	{
		DrawCircle(Game::screen_width - 400 + (i * 70), 100, 30, 0xff0000, true);
	}

	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// ���f���̕`��
	pModel_->Draw();
}

float Player::GetColRadius()
{
	return col_radius;
}

void Player::OnDamage(int damage)
{
	if (damageFrame_ > 0)	return;

	hp_ -= damage;
	damageFrame_ = invincible_time;
}

void Player::SetVisible(bool visible)
{
	if (visible)
	{
		MV1SetFrameVisible(pModel_->GetModelHandle(), 43, true);
		MV1SetFrameVisible(pModel_->GetModelHandle(), 41, true);
		MV1SetFrameVisible(pModel_->GetModelHandle(), 42, true);
	}
	else
	{
		MV1SetFrameVisible(pModel_->GetModelHandle(), 43, false);
	//	MV1SetFrameVisible(pModel_->GetModelHandle(), 41, false);
		MV1SetFrameVisible(pModel_->GetModelHandle(), 42, false);
	}
}

void Player::UpdateIdle(const InputState& input)
{
	// �A�j���[�V������i�߂�
	pModel_->Update();

	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �W�����v����
	bool isJumping = true;	// �W�����v���Ă���t���O
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y < 0.0f)
	{
		pos_.y = 0.0f;
		jumpAcc_ = 0.0f;

		isJumping = false;
	}

	// �v���C���[�������Ă�������ɂ���Đi�ޕ��������߂�
	VECTOR vect = MV1GetRotationXYZ(pModel_->GetModelHandle());

	VECTOR moveZ = VTransform(player_vec_z, MGetRotY(pCamera_->GetCameraAngle()));
	VECTOR moveX = VTransform(player_vec_x, MGetRotY(pCamera_->GetCameraAngle() + vect.x));

	if (!isJumping)
	{
		// A�{�^���ŃW�����v
		if (input.IsTriggered(InputType::jump))
		{
			jumpAcc_ = jump_power;
		}
	}

	// �V���b�g��������
	if (input.IsPressed(InputType::shot))
	{
		// �e�̔��ˈʒu
		VECTOR shootStart = pos_;	
		shootStart.y = 80.0f;

		// ���e�B�N���̈ʒu�̎擾
		VECTOR shotVec = ConvScreenPosToWorldPos(VGet(pMainScene_->GetReticlePosX(), pMainScene_->GetReticlePosY(), 1.0f));

		// �I�_����n�_������
		shotVec = VSub(shotVec , pos_);
		
		// ���K��
		shotVec = VNorm(shotVec);

		// �X�s�[�h������
		shotVec = VScale(shotVec, shot_speed);

		// �V���b�g�J�n
		pMainScene_->StartShot(shootStart, shotVec);

		// �~�܂��Ă���ꍇ�Ƒ����Ă���ꍇ�ŕ���
	//	if (!isMoving_)
		{
			// �V���b�g�A�j���ɕύX����
			animNo_ = idle_shot_anim_no;
			pModel_->ChangeAnimation(animNo_, false, true, 4);

			updateFunc_ = &Player::UpdateIdleShot;
			frameCount_ = 0;
		}
		//else
		//{
		//	// �V���b�g���s�A�j���ɕύX
		//	animNo_ = walk_shot_anim_no;
		//	pModel_->ChangeAnimation(walk_shot_anim_no, true, false, 4);
		//}
	}

	// �ړ�
	isMoving_ = false;
	if (input.IsPressed(InputType::w))
	{
		pos_ = VAdd(pos_, moveZ);

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::a))
	{
		pos_ = VSub(pos_, moveX);

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::s))
	{
		pos_ = VSub(pos_, moveZ);

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::d))
	{
		pos_ = VAdd(pos_, moveX);

		isMoving_ = true;
	}

	if (isMoving_)
	{
		if (animNo_ == idle_anim_no)
		{
			// ���s�A�j���ɕύX
			animNo_ = walk_anim_no;
			pModel_->ChangeAnimation(walk_anim_no, true, false, 4);
		}
	}
	else
	{
		if (animNo_ == walk_anim_no)
		{
			// �ҋ@�A�j���ɕύX
			animNo_ = idle_anim_no;
			pModel_->ChangeAnimation(idle_anim_no, true, false, 4);
		}
		else if (animNo_ == walk_shot_anim_no)
		{
			// �ҋ@�A�j���ɕύX
			animNo_ = idle_anim_no;
			pModel_->ChangeAnimation(idle_anim_no, true, false, 4);
		}
	}

	pModel_->SetPos(pos_);
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraAngle(), 0.0f));
}

void Player::UpdateIdleShot(const InputState& input)
{
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y < 0.0f)
	{
		pos_.y = 0.0f;
		jumpAcc_ = 0.0f;
	}

	// �V���b�g�A�j���ȊO�ł���Update�͌Ă΂�Ȃ�
	assert(animNo_ == idle_shot_anim_no);
	pModel_->Update();

	frameCount_++;
	if (pModel_->IsAnimEnd())
	{
		// �ҋ@�A�j���ɕύX����
		animNo_ = idle_anim_no;
		pModel_->ChangeAnimation(idle_anim_no, true, true, 4);

		// Update��ҋ@��
		updateFunc_ = &Player::UpdateIdle;
		frameCount_ = 0;
	}
}
