#include "Collision.h"
#include "Object/FieldManager.h"
#include "Object/Field.h"

Collision::Collision()
{
}

Collision::~Collision()
{
}

void Collision::Init()
{
}

VECTOR Collision::ColisionToField(int modelHandle, bool isMove, bool isJump, VECTOR pos, VECTOR vec)
{
	for (auto& field : pFieldManager_->GetField())
	{
		MV1_COLL_RESULT_POLY* poly;
		HITRESULT_LINE lineRes;

		// ������
		bool hitFlag = false;
		int yukaNum = 0;
		int kabeNum = 0;

		// ���f���̍ő咸�_���W�ƍŏ����_���W�̎擾
		MV1_REF_POLYGONLIST refPoly = MV1GetReferenceMesh(modelHandle, -1, true);

		// �ړ��O�̃v���C���[�̍��W
		VECTOR oldPos = pos;

		// �ړ���̃v���C���[�̍��W
		moveAfterPos_ = VAdd(oldPos, vec);

		// �v���C���[�ƃt�B�[���h�̓����蔻��(�����̃|���S���Ɠ������Ă��邩)
		MV1_COLL_RESULT_POLY_DIM hitDim = MV1CollCheck_Sphere(field->GetModelHandle(), -1, oldPos, (refPoly.MaxPosition.y + 50.0f) + VSize(vec));

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
			if (isMove)
			{
				// �ǃ|���S���̐������J��Ԃ�
				for (i = 0; i < kabeNum; i++)
				{
					// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					poly = kabe_[i];

					// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == false) continue;

					// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
					hitFlag = true;

					// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
					{
						VECTOR SlideVec;	// �v���C���[���X���C�h������x�N�g��

						// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
						SlideVec = VCross(vec, poly->Normal);

						// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
						// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
						SlideVec = VCross(poly->Normal, SlideVec);

						// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
						moveAfterPos_ = VAdd(oldPos, SlideVec);
					}

					// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
					for (j = 0; j < kabeNum; j++)
					{
						// j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
						poly = kabe_[j];

						// �������Ă����烋�[�v���甲����
						if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
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
					if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE)
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
						if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == FALSE) continue;

						// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
						moveAfterPos_ = VAdd(moveAfterPos_, VScale(poly->Normal, 5.0f));

						// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
						for (j = 0; j < kabeNum; j++)
						{
							// �������Ă����烋�[�v�𔲂���
							poly = kabe_[j];
							if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
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
			// ���|���S���ɓ����������ǂ����̃t���O��|���Ă���
			hitFlag = false;

			// ��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
			maxY_ = 0.0f;

			// ���|���S���̐������J��Ԃ�
			for (int i = 0; i < yukaNum; i++)
			{
				// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				poly = yuka_[i];

				// �W�����v�����ǂ����ŏ����𕪊�
				if (isJump)
				{
					// �W�����v���̏ꍇ�͓��̐悩�瑫���菭���Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
					lineRes = HitCheck_Line_Triangle(VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), VAdd(moveAfterPos_, VGet(0.0f, -1.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}
				else
				{
					// �����Ă���ꍇ�͓��̐悩�炻�������Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�( �X�΂ŗ�����ԂɈڍs���Ă��܂�Ȃ��� )
					lineRes = HitCheck_Line_Triangle(VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), VAdd(moveAfterPos_, VGet(0.0f, -40.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}

				// �������Ă��Ȃ������牽�����Ȃ�
				if (!lineRes.HitFlag) continue;

				// ���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
				if (hitFlag && maxY_ > lineRes.Position.y) continue;

				// �|���S���ɓ��������t���O�𗧂Ă�
				hitFlag = true;

				// �ڐG�����x���W��ۑ�����
				maxY_ = lineRes.Position.y;
			}

			// ���|���S���ɓ����������ǂ����ŏ����𕪊�
			if (hitFlag)
			{
				//// ���������ꍇ ////

				if (!isJump)
				{
					// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
					moveAfterPos_.y = maxY_;
				}
			}
			else
			{
				//// �������Ă��Ȃ��ꍇ ////
				if (!isJump)
				{
					moveAfterPos_.y -= 20;
				}
			}
		}
		else
		{
			// 1�����t�B�[���h�|���S���Ɠ������Ă��Ȃ��ꍇ����
			moveAfterPos_.y -= 20;
		}

		// ���o�����v���C���[�̎��͂̃|���S�������J������
		MV1CollResultPolyDimTerminate(hitDim);
	}
	return moveAfterPos_;
}
