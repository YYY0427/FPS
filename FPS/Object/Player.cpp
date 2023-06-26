#include "Player.h"
#include "../Model.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Scene/MainScene.h"
#include "../Object/Field.h"
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

	// �V���b�g�̔��ˈʒu
	constexpr VECTOR shot_firing_init_pos{ -89.264f, 150.0f, -260.0f };

	// �V���b�g�̑��x
	constexpr float shot_speed = 100.0f;

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
	constexpr int max_hp = 100;

	// �_���[�W�H��������̖��G����
	constexpr int invincible_time = 60;

	// �V���b�g�̍Ďg�p�܂őҋ@�t���[����
	constexpr int shot_wait_time = 5;
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
	moveVec_(VGet(0, 0, 0)),
	shotFrameCount_(0),
	isJump_(false),
	pMainScene_(nullptr)
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

void Player::CollisionField()
{
	for (auto& field : pField_)
	{
		MV1_COLL_RESULT_POLY* poly;
		MV1_COLL_RESULT_POLY_DIM hitDim;
		HITRESULT_LINE lineRes;
		MV1_REF_POLYGONLIST refPoly = MV1GetReferenceMesh(pModel_->GetModelHandle(), -1, true);

		// ������
		bool hitFlag = false;
		int yukaNum = 0;
		int kabeNum = 0;

		// �ړ��O�̃v���C���[�̍��W
		VECTOR oldPos = pos_;

		// �ړ���̃v���C���[�̍��W
		VECTOR moveAfterPos = VAdd(oldPos, moveVec_);

		// �v���C���[�ƃt�B�[���h�̓����蔻��(�����̃|���S���Ɠ������Ă��邩)
		hitDim = MV1CollCheck_Sphere(field->GetModelHandle(), -1, oldPos, (refPoly.MaxPosition.y + 50.0f) + VSize(moveVec_));

		// ���o���ꂽ�|���S���̐������J��Ԃ�
		for (int i = 0; i < hitDim.HitNum; i++)
		{
			// �w�y���ʂɐ������ǂ����̓|���S���̖@���̂x�������O�Ɍ���Ȃ��߂����ǂ����Ŕ��f����
			if (hitDim.Dim[i].Normal.y < 0.000001f && hitDim.Dim[i].Normal.y > -0.000001f)
			{
				// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{�P�D�O����荂���|���S���̂ݓ����蔻����s��
				if (hitDim.Dim[i].Position[0].y > oldPos.y + 1.0f ||
					hitDim.Dim[i].Position[1].y > oldPos.y + 1.0f ||
					hitDim.Dim[i].Position[2].y > oldPos.y + 1.0f)
				{
					// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
					if (kabeNum < 2048)
					{
						// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
						kabe_[kabeNum] = &hitDim.Dim[i];

						// �ǃ|���S���̐������Z����
						kabeNum++;
					}
				}
			}
			else
			{
				// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
				if (yukaNum < 2048)
				{
					// �|���S���̍\���̂̃A�h���X�����|���S���|�C���^�z��ɕۑ�����
					yuka_[yukaNum] = &hitDim.Dim[i];

					// ���|���S���̐������Z����
					yukaNum++;
				}
			}
		}

		// �ǃ|���S���Ƃ̓����蔻�菈��
		if (kabeNum != 0)
		{
			// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
			hitFlag = false;

			int i, j = 0;

			// �ړ��������ǂ����ŏ����𕪊�
			if (isMoving_)
			{
				// �ǃ|���S���̐������J��Ԃ�
				for (i = 0; i < kabeNum; i++)
				{
					// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					poly = kabe_[i];

					// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == false) continue;

					// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
					hitFlag = true;

					// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
					{
						VECTOR SlideVec;	// �v���C���[���X���C�h������x�N�g��

						// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
						SlideVec = VCross(moveVec_, poly->Normal);

						// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
						// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
						SlideVec = VCross(poly->Normal, SlideVec);

						// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
						moveAfterPos = VAdd(oldPos, SlideVec);
					}

					// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
					for (j = 0; j < kabeNum; j++)
					{
						// j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
						poly = kabe_[j];

						// �������Ă����烋�[�v���甲����
						if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
					}

					// j �� KabeNum �������ꍇ�͂ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
					// �ǂɓ��������t���O��|������Ń��[�v���甲����
					if (j == kabeNum)
					{
						hitFlag = false;
						break;
					}
				}
			}
			else
			{
				// �ړ����Ă��Ȃ��ꍇ�̏���

				// �ǃ|���S���̐������J��Ԃ�
				for (i = 0; i < kabeNum; i++)
				{
					// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					poly = kabe_[i];

					// �|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
					if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE)
					{
						hitFlag = true;
						break;
					}
				}
			}

			// �ǂɓ������Ă�����ǂ��牟���o���������s��
			if (hitFlag)
			{
				int k, i, j;

				// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
				for (k = 0; k < 16; k++)
				{
					// �ǃ|���S���̐������J��Ԃ�
					for (i = 0; i < kabeNum; i++)
					{
						// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
						poly = kabe_[i];

						// �v���C���[�Ɠ������Ă��邩�𔻒�
						if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == FALSE) continue;

						// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
						moveAfterPos = VAdd(moveAfterPos, VScale(poly->Normal, 5.0f));

						// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
						for (j = 0; j < kabeNum; j++)
						{
							// �������Ă����烋�[�v�𔲂���
							poly = kabe_[j];
							if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
						}

						// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
						if (j == kabeNum) break;
					}

					// i �� KabeNum �ł͂Ȃ��ꍇ�͑S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
					if (i != kabeNum) break;
				}
			}
		}


		// ���|���S���Ƃ̓����蔻��
		if (yukaNum != 0)
		{
			float MaxY;

			// ���|���S���ɓ����������ǂ����̃t���O��|���Ă���
			hitFlag = false;

			// ��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
			MaxY = 0.0f;

			// ���|���S���̐������J��Ԃ�
			for (int i = 0; i < yukaNum; i++)
			{
				// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				poly = yuka_[i];

				// �W�����v�����ǂ����ŏ����𕪊�
				if (isJump_)
				{
					// �W�����v���̏ꍇ�͓��̐悩�瑫���菭���Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
					lineRes = HitCheck_Line_Triangle(VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), VAdd(moveAfterPos, VGet(0.0f, -1.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}
				else
				{
					// �����Ă���ꍇ�͓��̐悩�炻�������Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�( �X�΂ŗ�����ԂɈڍs���Ă��܂�Ȃ��� )
					lineRes = HitCheck_Line_Triangle(VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), VAdd(moveAfterPos, VGet(0.0f, -40.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}

				// �������Ă��Ȃ������牽�����Ȃ�
				if (!lineRes.HitFlag) continue;

				// ���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
				if (hitFlag && MaxY > lineRes.Position.y) continue;

				// �|���S���ɓ��������t���O�𗧂Ă�
				hitFlag = true;

				// �ڐG�����x���W��ۑ�����
				MaxY = lineRes.Position.y;
			}

			// ���|���S���ɓ����������ǂ����ŏ����𕪊�
			if (hitFlag)
			{
				//// ���������ꍇ ////

				// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
				moveAfterPos.y = MaxY;
			}
			else
			{
				//// �������Ă��Ȃ��ꍇ ////

				moveAfterPos.y -= 20;
			}
		}
		else
		{
			// 1�����t�B�[���h�|���S���Ɠ������Ă��Ȃ��ꍇ����
			moveAfterPos.y -= 20;
		}

		pos_ = moveAfterPos;

		// ���o�����v���C���[�̎��͂̃|���S�������J������
		MV1CollResultPolyDimTerminate(hitDim);
	}
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
	// �t���[���J�E���g
	shotFrameCount_++;

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	//// �W�����v����
	//jumpAcc_ += gravity;
	//pos_.y += jumpAcc_;
	//if (pos_.y < 0.0f)
	//{
	////	pos_.y = 0.0f;
	//	jumpAcc_ = 0.0f;

	//	isJump_ = false;
	//}

	// �W�����v����
	if (!isJump_)
	{
		if (input.IsTriggered(InputType::jump))
		{
			jumpAcc_ = jump_power;
			isJump_ = true;
		}
	}

	// �V���b�g��������(�{�^���������ꂽ�Ƃ��A)
	if (input.IsPressed(InputType::shot) && shotFrameCount_ >= shot_wait_time)
	{
		// �e�̔��ˈʒu�̍쐬
		MATRIX playerTransMtx = MGetTranslate(pos_);						// �v���C���[�̕��s�ړ��s��̍쐬
		MATRIX cameraRotMtxSide = MGetRotY(pCamera_->GetCameraAngleX());	// ���ړ����̍s��쐬		
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
		pMainScene_->StartShot(shootStartPos, shotVec);

		// �V���b�g�A�j���ɕύX����
		animNo_ = idle_shot_anim_no;
		pModel_->ChangeAnimation(animNo_, false, true, 4);

		// ������
		shotFrameCount_ = 0;
	}

	// �v���C���[�̉�]�l���擾����
	VECTOR vect = MV1GetRotationXYZ(pModel_->GetModelHandle());

	// �J�����������Ă����������x�N�g���ϊ����Ĉړ����쐬
	VECTOR moveUp = VTransform(player_vec_up, MGetRotY(pCamera_->GetCameraAngleX()));
	VECTOR moveDown = VTransform(player_vec_down, MGetRotY(pCamera_->GetCameraAngleX()));
	VECTOR moveRight = VTransform(player_vec_right, MGetRotY(pCamera_->GetCameraAngleX() + vect.x));
	VECTOR moveLeft = VTransform(player_vec_left, MGetRotY(pCamera_->GetCameraAngleX() + vect.x));

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
	pos_ = pMainScene_->ColisionToField(pModel_->GetModelHandle(), isMoving_, isJump_, pos_, moveVec_);

	// �V���b�g�A�j�����I��莟��ҋ@�A�j���ɕύX
	if (pModel_->IsAnimEnd() && animNo_ == idle_shot_anim_no)
	{
		animNo_ = idle_anim_no;
		pModel_->ChangeAnimation(idle_anim_no, true, true, 4);
	}

	// �A�j���[�V������i�߂�
	pModel_->Update();

	// �ʒu���W�̐ݒ�
	pModel_->SetPos(pos_);

	// �����Ă�������̐ݒ�
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraAngleX(), 0.0f));
}

void Player::UpdateIdleShot(const InputState& input)
{
	// �V���b�g�A�j���ȊO�ł���Update�͌Ă΂�Ȃ�
	assert(animNo_ == idle_shot_anim_no);

	// �_���[�W����
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// �A�j���[�V�����X�V����
	pModel_->Update();

	//// �W�����v����
	//jumpAcc_ += gravity;
	//pos_.y += jumpAcc_;
	//if (pos_.y < 0.0f)
	//{
	////	pos_.y = 0.0f;
	//	jumpAcc_ = 0.0f;
	//}

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

	//// �W�����v����
	//jumpAcc_ += gravity;
	//pos_.y += jumpAcc_;
	//if (pos_.y < 0.0f)
	//{
	////	pos_.y = 0.0f;
	//	jumpAcc_ = 0.0f;
	//}
}

void Player::UpdateOnDamage(const InputState& input)
{
	assert(animNo_ == damage_anim_no);
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
