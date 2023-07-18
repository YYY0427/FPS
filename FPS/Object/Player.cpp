#include "Player.h"
#include "../Model.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Scene/MainScene.h"
#include "../Collision.h"
#include "Tower.h"
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
	constexpr float player_speed = 15.0f;

	// �V���b�g�̔��ˈʒu
	constexpr VECTOR shot_firing_init_pos{ -89.264f, 150.0f, -260.0f };

	// �V���b�g�̑��x
	constexpr float shot_speed = 100.0f;

	// �W�����v��
	constexpr float jump_power = 45.0f;

	// �d��
	constexpr float gravity = -1.0f;

	// �A�j���[�V�����ԍ�
	constexpr int idle_anim_no = 3;			// �ҋ@���[�V����
	constexpr int idle_shot_anim_no = 5;	// ��~���Ă����ԂŃV���b�g������
	constexpr int walk_anim_no = 14;		// �ړ����[�V����	
	constexpr int punch_anim_no = 10;		// �e�ŉ��郂�[�V����
	constexpr int walk_shot_anim_no = 13;	// �ړ����Ă����ԂŃV���b�g������
	constexpr int dead_anim_no = 0;			// ���S�A�j���[�V����
	constexpr int damage_anim_no = 2;		// �_���[�W�A�j���[�V����
	constexpr int fall_anim_no = 7;			// �����A�j���[�V����

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

	// �V���b�g�̍Ďg�p�܂őҋ@�t���[����
	constexpr int attack_wait_time = 5;

	// ���X�|�[���n�_
	constexpr VECTOR respawn_point{ 6000.0f, 0.0f, 2200.0f };

	// �����ʒu
	constexpr VECTOR bee_init_pos_1{ 6000.0f, 0.0f, 2200.0f };

	// ���x�i1=1m�A60fps�Œ�Ƃ��āA����10km�j
	// 10000m �� ���� �� �� �� �b �� �t���[��
//	constexpr float player_speed = 10000.0f / 60.0f / 60.0f / 60.0f;
}

Player::Player(MainScene* pMainScene) :
	pMainScene_(pMainScene),
	updateFunc_(&Player::UpdateIdle),
	animNo_(idle_anim_no),
	frameCount_(0),
	pos_(VGet(0, 0, 0)),
	jumpAcc_(0.0f),
	hp_(max_hp),
	damageFrame_(0),
	isMoving_(false),
	isDead_(false),
	moveVec_(VGet(0, 0, 0)),
	shotFrameCount_(0),
	isJump_(false),
	pCollision_(nullptr),
	isFall_(false)
{
	// 3D���f���̐���
	pModel_ = std::make_shared<Model>(file_name);

	// �A�j���[�V�����ݒ�
	pModel_->SetAnimation(animNo_, true, true);
	pModel_->SetUseCollision(true);

	// �����ʒu
	pos_ = bee_init_pos_1;
	pModel_->SetPos(pos_);
	pModel_->Update();
}

Player::~Player()
{
	
}

void Player::Update(const InputState& input)
{
	(this->*updateFunc_)(input);
}

void Player::Draw()
{
	// �_���[�W����
	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// ���f���̕`��
	pModel_->Draw();

	// HP�̕`��
	for (int i = 0; i < hp_; i++)
	{
		DrawCircle(100 + (i * 70), 130, 30, 0xff0000, true);
	}

#ifdef _DEBUG
	DrawFormatString(20, 200, 0x000000, "playerMinY = %f", pCollision_->GetPlayerMinY());
	DrawFormatString(20, 300, 0x000000, "playerPos = %f, %f, %f", pos_.x, pos_.y, pos_.z);
	DrawSphere3D(pos_, col_radius, 16.0f, 0xff0000, 0xff0000, false);
#endif
}

void Player::SetRespawn()
{
	isFall_ = false;
	OnDamage(1);
	pos_ = respawn_point;
}

float Player::GetColRadius() const
{
	return col_radius;
}

void Player::OnDamage(int damage)
{
	if (damageFrame_ > 0)	return;
	if (pTower_->GetIsDead()) return;

	// �_���[�W����
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
	// �t���[���J�E���g
	shotFrameCount_++;

	// �^���[������ł�����v���C���[�𓮂����Ȃ�
	if (pTower_->GetIsDead())
	{
		animNo_ = dead_anim_no;
		updateFunc_ = &Player::UpdateDead;
	}

	// �n�ʂ��痎�����ꍇ�_���[�W�����炢�A���X�|�[���n�_�ɖ߂�
	if (pos_.y < pCollision_->GetPlayerMinY() - 2000.0f)
	{
		isFall_ = true;
		pMainScene_->PlayerFallFade();
	}

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �W�����v����
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y <= pCollision_->GetPlayerMinY())
	{
	//	pos_.y = pMainScene_->GetMaxY();
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}

	// �W�����v����
	if (!isJump_)
	{
		if (input.IsPressed(InputType::jump))
		{
			jumpAcc_ = jump_power;
			isJump_ = true;
		}
	}

	// �V���b�g��������(�{�^���������ꂽ�Ƃ��A)
	if (input.IsPressed(InputType::shot) && shotFrameCount_ >= attack_wait_time)
	{
		// �e�̔��ˈʒu�̍쐬
		MATRIX playerTransMtx = MGetTranslate(pos_);						// �v���C���[�̕��s�ړ��s��̍쐬
		MATRIX cameraRotMtxSide = MGetRotY(pCamera_->GetCameraYaw());	// ���ړ����̍s��쐬		
		MATRIX matrix = MMult(cameraRotMtxSide, playerTransMtx);			// ���ړ����s��ƃv���C���[�̕��s�ړ��s��̍���
		VECTOR shootStartPos = VTransform(shot_firing_init_pos, matrix);	// �V���b�g�̔��ˏ����ʒu�ƍ쐬�����s�񂩂�x�N�g���̐���

		// ���e�B�N���̈ʒu�̎擾
		VECTOR shotVec = ConvScreenPosToWorldPos(VGet(pMainScene_->GetReticlePosX(), pMainScene_->GetReticlePosY(), 1.0f));

		// �I�_����n�_������
		shotVec = VSub(shotVec, shootStartPos);

		// ���K��
		shotVec = VNorm(shotVec);

		// �X�s�[�h������
		shotVec = VScale(shotVec, shot_speed);

		// �V���b�g�J�n
		pMainScene_->StartPlayerShot(shootStartPos, shotVec);

		// �V���b�g�A�j���ɕύX����
		animNo_ = idle_shot_anim_no;
		pModel_->ChangeAnimation(animNo_, false, true, 4);

		// ������
		shotFrameCount_ = 0;
	}

	// �v���C���[�̉�]�l���擾����
	VECTOR vect = MV1GetRotationXYZ(pModel_->GetModelHandle());

	// �J�����������Ă����������x�N�g���ϊ����Ĉړ����쐬
	VECTOR moveUp = VTransform(player_vec_up, MGetRotY(pCamera_->GetCameraYaw()));
	VECTOR moveDown = VTransform(player_vec_down, MGetRotY(pCamera_->GetCameraYaw()));
	VECTOR moveRight = VTransform(player_vec_right, MGetRotY(pCamera_->GetCameraYaw() + vect.x));
	VECTOR moveLeft = VTransform(player_vec_left, MGetRotY(pCamera_->GetCameraYaw() + vect.x));

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
	}

	// �����蔻��`�F�b�N
	pos_ = pCollision_->Colision(pModel_->GetModelHandle(), isMoving_, isJump_, pos_, moveVec_, Collision::Chara::player);
//	pos_ = pCollision_->ColisionToTower(pModel_->GetModelHandle(), isMoving_, isJump_, pos_, moveVec_);

	// �V���b�g�A�j�����I��莟��ҋ@�A�j���ɕύX
	if (pModel_->IsAnimEnd() && animNo_ == idle_shot_anim_no)
	{
		animNo_ = idle_anim_no;
		pModel_->ChangeAnimation(idle_anim_no, true, true, 4);
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraYaw(), 0.0f));

	// �A�j���[�V������i�߂�
	pModel_->Update();
}

void Player::UpdateIdleShot(const InputState& input)
{
	// �V���b�g�A�j���ȊO�ł���Update�͌Ă΂�Ȃ�
	assert(animNo_ == idle_shot_anim_no);

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �W�����v����
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y <= pCollision_->GetPlayerMinY())
	{
	//	pos_.y = pMainScene_->GetMaxY();
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraYaw(), 0.0f));

	// �A�j���[�V������i�߂�
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
	if (pos_.y <= pCollision_->GetPlayerMinY())
	{
		pos_.y = pCollision_->GetPlayerMinY();
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}
}

void Player::UpdateOnDamage(const InputState& input)
{
	assert(animNo_ == damage_anim_no);

	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraYaw(), 0.0f));

	// �A�j���[�V������i�߂�
	pModel_->Update();

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
