#include "Collision.h"
#include "StageManager.h"
#include "StageBase.h"
#include "Object/Tower.h"
#include "Object/EnemyManager.h"
#include "Object/EnemyBase.h"
#include "Obstacle.h"
#include "ObstacleManager.h"
#include "Object/Player.h"

Collision::Collision(StageManager* pStages, std::shared_ptr<Tower> pTower, std::shared_ptr<EnemyManager> pEnemyManager, std::shared_ptr<ObstacleManager> pObstacleManager, std::shared_ptr<Player> pPlayer) :
	moveAfterPos_(VGet(0, 0, 0)),
	oldPos_(VGet(0, 0, 0)),
	yukaNum_(0),
	kabeNum_(0),
	isHitFlag_(false),
	pStages_(pStages),
	pTower_(pTower),
	pEnemyManager_(pEnemyManager),
	pObstacleManager_(pObstacleManager),
	pPlayer_(pPlayer)
{
}

Collision::~Collision()
{
}

void Collision::Init()
{
	moveAfterPos_ = VGet(0, 0, 0);
	oldPos_ = VGet(0, 0, 0);
	yukaNum_ = 0;
	kabeNum_ = 0;
	isHitFlag_ = false;
}

void Collision::CollCheck(int characterModelHandle, int objectModelHandle, int collisionFrameIndex, VECTOR pos, VECTOR vec, float collisionRadius, int chara)
{
	// ���f���̍ő咸�_���W�ƍŏ����_���W�̎擾
	refPoly_ = MV1GetReferenceMesh(characterModelHandle, -1, true);

	// �ړ��O�̍��W
	oldPos_ = pos;

	// �ړ���̍��W
	moveAfterPos_ = VAdd(oldPos_, vec);

	// ���f���ƃt�B�[���h�̓����蔻��(�����̃|���S���Ɠ������Ă��邩)
//	hitDim_ = MV1CollCheck_Sphere(objectModelHandle, -1, oldPos_, (refPoly_.MaxPosition.y + 50.0f) + VSize(vec));
	hitDim_ = MV1CollCheck_Capsule(objectModelHandle, collisionFrameIndex, oldPos_, VGet(oldPos_.x, oldPos_.y + refPoly_.MaxPosition.y, oldPos_.z), collisionRadius);

	// ���o���ꂽ�|���S���̐������J��Ԃ�
	for (int i = 0; i < hitDim_.HitNum; i++)
	{
		// �w�y���ʂɐ������ǂ����̓|���S���̖@���̂x�������O�Ɍ���Ȃ��߂����ǂ����Ŕ��f����
	//	if (hitDim_.Dim[i].Normal.y < 0.000001f && hitDim_.Dim[i].Normal.y > -0.000001f)
		if (hitDim_.Dim[i].Normal.y < 0.4f && hitDim_.Dim[i].Normal.y > -0.000001f)
		{
			// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{�P�D�O����荂���|���S���̂ݓ����蔻����s��
			if (hitDim_.Dim[i].Position[0].y > oldPos_.y + 1.0f ||
				hitDim_.Dim[i].Position[1].y > oldPos_.y + 1.0f ||
				hitDim_.Dim[i].Position[2].y > oldPos_.y + 1.0f)
			{
				// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
				if (kabeNum_ < 2048)
				{
					// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
					kabe_[kabeNum_] = &hitDim_.Dim[i];

					// �ǃ|���S���̐������Z����
					kabeNum_++;
				}
			}
		}
		else
		{
			// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
			if (yukaNum_ < 2048)
			{
				// �|���S���̍\���̂̃A�h���X�����|���S���|�C���^�z��ɕۑ�����
				yuka_[yukaNum_] = &hitDim_.Dim[i];

				// ���|���S���̐������Z����
				yukaNum_++;
			}
		}
	}
}

void Collision::WallPolyColCheckProcess(bool isMove, VECTOR vec)
{
	// �ǃ|���S���Ƃ̓����蔻�菈��
	if (kabeNum_ != 0)
	{
		// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
		isHitFlag_ = false;

		int i, j = 0;

		// �ړ��������ǂ����ŏ����𕪊�
		if (isMove)
		{
			// �ǃ|���S���̐������J��Ԃ�
			for (i = 0; i < kabeNum_; i++)
			{
				// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				poly_ = kabe_[i];

				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
				if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == FALSE) continue;

				// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
				isHitFlag_ = true;

				// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
				{
					// �v���C���[���X���C�h������x�N�g��
					VECTOR SlideVec;	

					// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
					SlideVec = VCross(vec, poly_->Normal);

					// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
					// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
					SlideVec = VCross(poly_->Normal, SlideVec);

					// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
					moveAfterPos_ = VAdd(oldPos_, SlideVec);
				}

				// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
				for (j = 0; j < kabeNum_; j++)
				{
					// j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					poly_ = kabe_[j];

					// �������Ă����烋�[�v���甲����
					if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == TRUE) break;
				}

				// j �� KabeNum �������ꍇ�͂ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
				// �ǂɓ��������t���O��|������Ń��[�v���甲����
				if (j == kabeNum_)
				{
					isHitFlag_ = false;
					break;
				}
			}
		}
		else
		{
			// �ړ����Ă��Ȃ��ꍇ�̏���

			// �ǃ|���S���̐������J��Ԃ�
			for (i = 0; i < kabeNum_; i++)
			{
				// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				poly_ = kabe_[i];

				// �|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
				if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == TRUE)
				{
					isHitFlag_ = true;
					break;
				}
			}
		}

		// �ǂɓ������Ă�����ǂ��牟���o���������s��
		if (isHitFlag_)
		{
			int k, i, j;

			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
			for (k = 0; k < 32; k++)
			{
				// �ǃ|���S���̐������J��Ԃ�
				for (i = 0; i < kabeNum_; i++)
				{
					// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					poly_ = kabe_[i];

					// �v���C���[�Ɠ������Ă��邩�𔻒�
					if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == FALSE) continue;

					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
				//	moveAfterPos_ = VAdd(moveAfterPos_, VScale(poly_->Normal, 5.0f));
					moveAfterPos_ = VAdd(moveAfterPos_, VScale(poly_->Normal, 0.1f));

					// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
					for (j = 0; j < kabeNum_; j++)
					{
						// �������Ă����烋�[�v�𔲂���
						poly_ = kabe_[j];
						if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == TRUE) break;
					}

					// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
					if (j == kabeNum_) break;
				}

				// i �� KabeNum �ł͂Ȃ��ꍇ�͑S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
				if (i != kabeNum_) break;
			}
		}
	}
}

void Collision::FloorPolyColCheckProcess(bool isJump, bool isUseGravity, int chara)
{
	// ���|���S���Ƃ̓����蔻��
	if (yukaNum_ != 0)
	{
		// ���|���S���ɓ����������ǂ����̃t���O��|���Ă���
		isHitFlag_ = false;

		// ��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
		float minY = 0.0f;

		// ���|���S���̐������J��Ԃ�
		for (int i = 0; i < yukaNum_; i++)
		{
			// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
			poly_ = yuka_[i];

			// �W�����v�����ǂ����ŏ����𕪊�
			if (isJump)
			{
				// �W�����v���̏ꍇ�͓��̐悩�瑫���菭���Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
				lineRes_ = HitCheck_Line_Triangle(VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), VAdd(moveAfterPos_, VGet(0.0f, -1.0f, 0.0f)), poly_->Position[0], poly_->Position[1], poly_->Position[2]);
			}
			else
			{
				// �����Ă���ꍇ�͓��̐悩�炻�������Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�( �X�΂ŗ�����ԂɈڍs���Ă��܂�Ȃ��� )
				lineRes_ = HitCheck_Line_Triangle(VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), VAdd(moveAfterPos_, VGet(0.0f, -40.0f, 0.0f)), poly_->Position[0], poly_->Position[1], poly_->Position[2]);
			}

			// �������Ă��Ȃ������牽�����Ȃ�
			if (!lineRes_.HitFlag) continue;

			// ���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
			if (isHitFlag_ && minY > lineRes_.Position.y) continue;

			// �|���S���ɓ��������t���O�𗧂Ă�
			isHitFlag_ = true;

			// �ڐG�����x���W��ۑ�����
			minY = lineRes_.Position.y;
		}

		// ���|���S���ɓ����������ǂ����ŏ����𕪊�
		if (isHitFlag_)
		{
			//// ���������ꍇ ////
			
			// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
			moveAfterPos_.y = minY;	

			// �ڐG�������|���S����ۑ�
			groundY_ = minY;
		}
		else
		{
			//// �������Ă��Ȃ��ꍇ ////

			if (isUseGravity)
			{
				// ���|���S���Ɠ������Ă��Ȃ��ꍇ����
				moveAfterPos_.y -= 20;
			}

			// �n�ʂ����݂��Ȃ��̂ł��肦�Ȃ��l����
			groundY_ = -10000.0f;
		}
	}
	else
	{
		//// �������Ă��Ȃ��ꍇ ////

		if (isUseGravity)
		{
			// ���|���S���Ɠ������Ă��Ȃ��ꍇ����
			moveAfterPos_.y -= 20;
		}

		// �n�ʂ����݂��Ȃ��̂ł��肦�Ȃ��l����
		groundY_ = -10000.0f;
	}
}

void Collision::JumpingFloorPolyColCheckProcess()
{
}

VECTOR Collision::Colision(int modelHandle, bool isMove, bool isJump, bool isUseGravity, VECTOR pos, VECTOR vec, int chara, float collisionRadius)
{
	// ������
	Init();

	// �^���[�Ƃ̓����蔻��`�F�b�N
	CollCheck(modelHandle, pTower_->GetModelHandle(), -1, pos, vec, collisionRadius, chara);

	for (auto& obj : pObstacleManager_->GetObstacles())
	{
		// ��Q���Ƃ̓����蔻��`�F�b�N
		CollCheck(modelHandle, obj->GetModelHandle(), -1, pos, vec, collisionRadius, chara);
	}

	for (auto& enemy : pEnemyManager_->GetEnemies())
	{
		if (enemy->GetModelHandle() == modelHandle) continue;

		// �G���m�̓����蔻��`�F�b�N
		CollCheck(modelHandle, enemy->GetModelHandle(), enemy->GetColFrameIndex(), pos, vec, collisionRadius, chara);
	}

	// �t�B�[���h�Ƃ̓����蔻��`�F�b�N
	CollCheck(modelHandle, pStages_->GetStages()->GetModelHandle(), -1, pos, vec, collisionRadius, chara);

	if (tower != chara)
	{
		// �ǃ|���S������
		WallPolyColCheckProcess(isMove, vec);
	}

	// ���|���S������
	FloorPolyColCheckProcess(isJump, isUseGravity, chara);

	// ���o�����v���C���[�̎��͂̃|���S�������J������
	MV1CollResultPolyDimTerminate(hitDim_);

	return moveAfterPos_;
}