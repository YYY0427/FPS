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
	constexpr VECTOR player_vec_up{ 0, 0, -1 };
	constexpr VECTOR player_vec_down{ 0, 0, 1 };
	constexpr VECTOR player_vec_right{ -1, 0, 0 };
	constexpr VECTOR player_vec_left{ 1, 0, 0 };

	// �v���C���[�̑��x
	constexpr float player_speed = 20.0f;

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
	constexpr int dead_anim_no = 0;			// ���S�A�j���[�V����
	constexpr int damage_anim_no = 2;		// ���S�A�j���[�V����

	// �A�j���[�V�����؂�ւ��ɂ�����t���[����
	constexpr int anim_change_frame = 16;

	// �t���[���ԍ�
	constexpr int body_frame_no = 41;	// �̂̃t���[��
	constexpr int ears_frame_no = 42;	// ���̃t���[��
	constexpr int head_frame_no = 43;	// ���̃t���[��

	// �����蔼�a�̃T�C�Y
	constexpr float col_radius = 70.0f;

	// �ő�HP
	constexpr int max_hp = 1;

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
	damageFrame_(0),
	isMoving_(false),
	isDead_(false),
	moveVec_(VGet(0, 0, 0))
{
}

Player::~Player()
{
}

void Player::Init()
{
	// 3D���f���̐���
	pModel_ = std::make_shared<Model>(file_name);
	pModel_->SetAnimation(animNo_, true, true);
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

	// �_���[�W����
	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// ���f���̕`��
	pModel_->Draw();
}

float Player::GetColRadius() const
{
	return col_radius;
}

void Player::OnDamage(int damage)
{
	// �_���[�W����
	if (damageFrame_ > 0)	return;
	hp_ -= damage;
	damageFrame_ = invincible_time;

	if (hp_ > 0)
	{
		// �A�j���[�V�������_���[�W�A�j���[�V�����ɕύX
		animNo_ = damage_anim_no;
		pModel_->ChangeAnimation(damage_anim_no, false, false, 4);
		updateFunc_ = &Player::UpdateOnDamage;
		frameCount_ = 0;
	}
	else
	{
		// ���S�t���O�𗧂Ă�
		isDead_ = true;

		// �A�j���[�V���������S�A�j���[�V�����ɕύX
		animNo_ = dead_anim_no;
		pModel_->ChangeAnimation(dead_anim_no, false, false, 4);
		updateFunc_ = &Player::UpdateDead;
		frameCount_ = 0;
	}
}

void Player::SetVisible(bool visible)
{
	// true�̏ꍇ�\���Afalse�̏ꍇ��\��
	if (visible)
	{
		// �\��
		MV1SetFrameVisible(pModel_->GetModelHandle(), ears_frame_no, true); // ��
		MV1SetFrameVisible(pModel_->GetModelHandle(), head_frame_no, true);	// ��
	}
	else
	{
		// ��\��
		MV1SetFrameVisible(pModel_->GetModelHandle(), ears_frame_no, false); // ��
		MV1SetFrameVisible(pModel_->GetModelHandle(), head_frame_no, false); // ��	
	}
}

void Player::UpdateIdle(const InputState& input)
{
	// �A�j���[�V������i�߂�
	pModel_->Update();

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �W�����v����
	bool isJumping = true;	
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y < 0.0f)
	{
		pos_.y = 0.0f;
		jumpAcc_ = 0.0f;

		isJumping = false;
	}

	// �W�����v����
	if (!isJumping)
	{
		if (input.IsTriggered(InputType::jump))
		{
			jumpAcc_ = jump_power;
		}
	}

	// �V���b�g��������
	if (input.IsPressed(InputType::shot))
	{
		// �e�̔��ˈʒu
		VECTOR shootStart = MV1GetFramePosition(pModel_->GetModelHandle(), 27);
		shootStart.y = 100.0f;

		// ���e�B�N���̈ʒu�̎擾
		VECTOR shotVec = ConvScreenPosToWorldPos(VGet(pMainScene_->GetReticlePosX(), pMainScene_->GetReticlePosY(), 1.0f));

		// �I�_����n�_������
		shotVec = VSub(shotVec, shootStart);

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

	// �v���C���[�̉�]�l���擾����
	VECTOR vect = MV1GetRotationXYZ(pModel_->GetModelHandle());

	// �J�����������Ă����������x�N�g���ϊ����Ĉړ����쐬
	VECTOR moveUp = VTransform(player_vec_up, MGetRotY(pCamera_->GetCameraAngle()));
	VECTOR moveDown = VTransform(player_vec_down, MGetRotY(pCamera_->GetCameraAngle()));
	VECTOR moveRight = VTransform(player_vec_right, MGetRotY(pCamera_->GetCameraAngle() + vect.x));
	VECTOR moveLeft = VTransform(player_vec_left, MGetRotY(pCamera_->GetCameraAngle() + vect.x));

	// �ړ�
	isMoving_ = false;
	moveVec_ = VGet(0, 0, 0);
	VECTOR moveVecX = VGet(0, 0, 0);
	VECTOR moveVecZ = VGet(0, 0, 0);
	if (input.IsPressed(InputType::w))
	{
		moveVecZ = moveUp;

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::a))
	{
		moveVecX = moveLeft;

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::s))
	{
		moveVecZ = moveDown;

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::d))
	{
		moveVecX = moveRight;

		isMoving_ = true;
	}

	if (isMoving_)
	{
		// x������z�����̃x�N�g���𑫂��Ĉړ��x�N�g�����쐬����
		moveVec_ = VAdd(moveVecZ, moveVecX);

		// ���K��
		moveVec_ = VNorm(moveVec_);

		// ���K�������x�N�g���Ƀv���C���[�̑��x��������
		moveVec_ = VScale(moveVec_, player_speed);

		// ������
		pos_ = VAdd(pos_, moveVec_);

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
	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �W�����v����
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

void Player::UpdateDead(const InputState& input)
{
	assert(animNo_ == dead_anim_no);
	pModel_->Update();

	// �W�����v����
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y < 0.0f)
	{
		pos_.y = 0.0f;
		jumpAcc_ = 0.0f;
	}

	if (pModel_->IsAnimEnd())
	{

	}
}

void Player::UpdateOnDamage(const InputState& input)
{
	assert(animNo_ == damage_anim_no);
	pModel_->Update();

	if (pModel_->IsAnimEnd())
	{
	//	if (!isMoving_)
		{
			// �ҋ@�A�j���ɕύX����
			animNo_ = idle_anim_no;
			pModel_->ChangeAnimation(idle_anim_no, true, true, 4);

			// Update��ҋ@��
			updateFunc_ = &Player::UpdateIdle;
			frameCount_ = 0;
		}
	//	else
		{

		}
	}
}
