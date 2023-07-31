#include "Player.h"
#include "../Model.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Scene/MainScene.h"
#include "../Collision.h"
#include "../BomManager.h"
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

	// �{���V���b�g�̑��x
	constexpr float bom_speed = 50.0f;

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
	constexpr float collision_radius = 70.0f;

	// �ő�HP
	constexpr int max_hp = 1000;

	// �_���[�W�H��������̖��G����
	constexpr int invincible_time = 60;

	// �V���b�g�̍Ďg�p�܂őҋ@�t���[����
	constexpr int shot_wait_time = 30;

	// ���e�̍Ďg�p�܂őҋ@�t���[����
	constexpr int bom_wait_time = 60 * 10;

	// ���X�|�[���n�_
	constexpr VECTOR respawn_point{ 6000.0f, 0.0f, 2200.0f };

	// �����ʒu
	constexpr VECTOR bee_init_pos_1{ 6000.0f, 0.0f, 2200.0f };
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
	isFall_(false),
	bomFrameCount_(0),
	isUseBom_(true),
	isUseShot_(true)
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

#ifdef _DEBUG
	DrawFormatString(20, 300, 0x000000, "playerPos = %f, %f, %f", pos_.x, pos_.y, pos_.z);
	DrawFormatString(20, 250, 0x000000, "playerGroundY = %f", pCollision_->GetGroundY());
	DrawSphere3D(pos_, collision_radius, 16.0f, 0xff0000, 0xff0000, false);
#endif
}

void Player::SetRespawn()
{
	isFall_ = false;
	OnDamage(1);
	pos_ = respawn_point;
}

int Player::GetMaxHP() const
{
	return max_hp;
}

float Player::GetCollisionRadius() const
{
	return collision_radius;
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
	shotFrameCount_--;
	if (shotFrameCount_ < 0) shotFrameCount_ = 0;
	if (shotFrameCount_ <= 0) isUseShot_ = true;

	bomFrameCount_--;
	if (bomFrameCount_ < 0) bomFrameCount_ = 0;
	if (bomFrameCount_ <= 0) isUseBom_ = true;

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �^���[������ł�����v���C���[�𓮂����Ȃ�
	if (pTower_->GetIsDead())
	{
		animNo_ = dead_anim_no;
		updateFunc_ = &Player::UpdateDead;
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

	// �V���b�g��������
	if (input.IsPressed(InputType::shot) && isUseShot_)
	{
		// �e�̔��ˈʒu�̍쐬
		MATRIX playerTransMtx = MGetTranslate(pos_);						// �v���C���[�̕��s�ړ��s��̍쐬
		MATRIX cameraRotMtxSide = MGetRotY(pCamera_->GetCameraYaw());		// ���ړ����̍s��쐬		
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
		shotFrameCount_ = shot_wait_time;
		isUseShot_ = false;
	}

	if (input.IsTriggered(InputType::bom) && /*bomFrameCount_ >= bom_wait_time*/isUseBom_)
	{
		// �e�̔��ˈʒu�̍쐬
		MATRIX playerTransMtx = MGetTranslate(pos_);						// �v���C���[�̕��s�ړ��s��̍쐬
		MATRIX cameraRotMtxSide = MGetRotY(pCamera_->GetCameraYaw());		// ���ړ����̍s��쐬		
		MATRIX matrix = MMult(cameraRotMtxSide, playerTransMtx);			// ���ړ����s��ƃv���C���[�̕��s�ړ��s��̍���
		VECTOR shootStartPos = VTransform(shot_firing_init_pos, matrix);	// �V���b�g�̔��ˏ����ʒu�ƍ쐬�����s�񂩂�x�N�g���̐���

		// ���e�B�N���̈ʒu�̎擾
		VECTOR shotVec = ConvScreenPosToWorldPos(VGet(pMainScene_->GetReticlePosX(), pMainScene_->GetReticlePosY(), 1.0f));

		// �I�_����n�_������
		shotVec = VSub(shotVec, shootStartPos);

		// ���K��
		shotVec = VNorm(shotVec);

		// �X�s�[�h������
		shotVec = VScale(shotVec, bom_speed);

		pBomManager_->StartBom(shootStartPos, shotVec, pCamera_->GetCameraYaw());

		// �V���b�g�A�j���ɕύX����
		animNo_ = idle_shot_anim_no;
		pModel_->ChangeAnimation(animNo_, false, true, 4);

		bomFrameCount_ = bom_wait_time;
		isUseBom_ = false;
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
	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), isMoving_, isJump_, true, pos_, moveVec_, Collision::Chara::player, collision_radius);

	// �W�����v����
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y < pCollision_->GetGroundY())
	{
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}

	// �n�ʂ��痎�����ꍇ�_���[�W�����炢�A���X�|�[���n�_�ɖ߂�
	if (pos_.y <  - 3000.0f)
	{
		isFall_ = true;
		pMainScene_->PlayerFallFade();
	}

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
	if (pos_.y <= pCollision_->GetGroundY())
	{
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
	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), isMoving_, false, true, pos_, VGet(0, 0, 0), Collision::Chara::player, collision_radius);
}

void Player::UpdateOnDamage(const InputState& input)
{
	assert(animNo_ == damage_anim_no);

	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), isMoving_, isJump_, true, pos_, moveVec_, Collision::Chara::player, collision_radius);

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
