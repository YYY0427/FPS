#include "MainScene.h"
#include "TitleScene.h"
#include "PauseScene.h"
#include "SceneManager.h"
#include "../Object/EnemyManager.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Object/Player.h"
#include "../Object/Field.h"
#include "../Object/Shot.h"
#include "../Object/Enemy.h"
#include "../DrawFunctions.h"
#include "../Object/SkyDoom.h"
#include "../Object/FieldManager.h"
#include <cassert>
#include <stdlib.h>

namespace
{
	// �t�F�[�h�̑��x
	constexpr int fade_interval = 30;

	// ��ʓ��ɑ��݂ł���e�̍ő吔
	constexpr int shot_max = 128;

	// �N���X�w�A�̈ʒu
	constexpr int reticle_pos_x = Game::screen_width / 2;
	constexpr int reticle_pos_y = Game::screen_height / 2;

	// �Q�[���I�[�o�[���̕\�������̃t�F�[�h���x
	constexpr int game_over_fade_interval = 60;
}

MainScene::MainScene(SceneManager& manager) :
	Scene(manager),
	updateFunc_(&MainScene::NormalUpdate),
	fadeTimer_(fade_interval),
	fadeValue_(255),
	shadowMap_(-1),
	gameOverUIhandle_(-1),
	gameOverFadeTimer_(0)
{
	pCamera_ = std::make_shared<Camera>();
	pPlayer_ = std::make_shared<Player>();
	pFieldManager_ = std::make_shared<FieldManager>();
	pEnemyManager_ = std::make_shared<EnemyManager>();
	pSkyDoom_ = std::make_shared<SkyDoom>();
	for (int i = 0; i < shot_max; i++)
	{
		pShot_.push_back(std::make_shared<Shot>());
	}

	Init();
}

MainScene::~MainScene()
{
	// �V���h�E�}�b�v�̍폜
	DeleteShadowMap(shadowMap_);
}

void MainScene::Init()
{
	// �������ƃ|�C���^��n��
	pPlayer_->SetMainScene(static_cast<std::shared_ptr<MainScene>>(this));
	pCamera_->SetPlayer(pPlayer_);
	pPlayer_->SetCamera(pCamera_);
	pSkyDoom_->SetPlayer(pPlayer_);
	pPlayer_->SetField(pFieldManager_->GetField());

	// 1�񂾂����f�������[�h���Ă�����g���ă��f���̕���
	int handle = pShot_[0]->LoadModel();
	for (auto& shot : pShot_)
	{
		shot->SetPlayer(pPlayer_);
		shot->SetCamera(pCamera_);
		shot->Init(handle);
	}
	pFieldManager_->Init();
	pPlayer_->Init();
	pEnemyManager_->Init();
	pCamera_->Init();
	pSkyDoom_->Init();

	for (auto& enemies : pEnemyManager_->GetEnemies())
	{
		enemies->SetPlayer(pPlayer_);
		enemies->SetMainScene(static_cast<std::shared_ptr<MainScene>>(this));
	}

	// �摜�̃��[�h
	gameOverUIhandle_ = my::MyLoadGraph("Data/Texture/youdead.png");

	// �V���h�E�}�b�v�̐���
	shadowMap_ = MakeShadowMap(1024, 1024);
	SetShadowMapLightDirection(shadowMap_, GetLightDirection());
}

void MainScene::Update(const InputState& input)
{
	(this->*updateFunc_)(input);
}

void MainScene::Draw()
{
	DrawString(0, 0, "MainScene", 0xffffff, true);

	pSkyDoom_->Draw();

	// �V���h�E�}�b�v�ւ̏�������
	ShadowMap_DrawSetup(shadowMap_);
	pFieldManager_->Draw();
	pPlayer_->Draw();
	pEnemyManager_->Draw();
	for (auto& shot : pShot_)
	{
		shot->Draw();
	}
	// �������ݏI��
	ShadowMap_DrawEnd();

	// �V���h�E�}�b�v���g�p���ă��f���̕`��
	SetUseShadowMap(0, shadowMap_);
	pFieldManager_->Draw();
	pPlayer_->Draw();
	pEnemyManager_->Draw();
	for (auto& shot : pShot_)
	{
		shot->Draw();
	}
	// �`��I��
	SetUseShadowMap(0, -1);

	// �G��HP�̕\��
	pEnemyManager_->DrawUI();

	// �v���C���[�����񂾂�\���J�n
	if (pPlayer_->GetIsDead())
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (gameOverUIfadeValue_ * 100) / 255);
		DrawGraph(0,  0, gameOverUIhandle_, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	// �v���C���[�������Ă���Ƃ��̂݃N���X�w�A��\��
	else
	{
		// �N���X�w�A
		DrawLine(reticle_pos_x - 25, reticle_pos_y, reticle_pos_x + 25, reticle_pos_y, 0xffffff);	// ��
		DrawLine(reticle_pos_x, reticle_pos_y - 25, reticle_pos_x, reticle_pos_y + 25, 0xffffff);	// �c
	}

#ifdef _DEBUG
	int cnt = 0;
	for (auto& shot : pShot_)
	{
		if (shot->isExist())
		{
			continue;
		}
		cnt++;
	}

	// �f�o�b�N�\��
	DrawFormatString(10, 90, 0x000000, "playerX = %f", pPlayer_->GetPos().x);
	DrawFormatString(10, 120, 0x000000, "playerY = %f", pPlayer_->GetPos().y);
	DrawFormatString(10, 150, 0x000000, "playerZ = %f", pPlayer_->GetPos().z);
	pCamera_->Draw();
	DrawFormatString(10, 210, 0x000000, "shotNum = %d", cnt);
#endif

	// �t�F�C�h
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeValue_);
	DrawBox(0, 0, Game::screen_width, Game::screen_height, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void MainScene::StartShot(VECTOR pos, VECTOR vec)
{
	for (auto& shot : pShot_)
	{
		// ��ʓ��ɂ��łɑ��݂��Ă����ꍇ�V���b�g���Ȃ�
		if (shot->isExist())
		{
			continue;
		}

		// ��ʂ�h�炷�x�������Z�b�g
		pCamera_->SetQuake(10, VGet(3, 0, 0));

		// �V���b�g�X�^�[�g
		shot->Start(pos, vec);
		break;
	}
}

float MainScene::GetReticlePosX() const
{
	return static_cast<float>(reticle_pos_x);
}

float MainScene::GetReticlePosY() const
{
	return static_cast<float>(reticle_pos_y);
}

VECTOR MainScene::ColisionToField(int modelHandle, bool isMove, bool isJump, VECTOR pos, VECTOR vec)
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
		VECTOR moveAfterPos = VAdd(oldPos, vec);

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
					if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == false) continue;

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
				if (isJump)
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

		// ���o�����v���C���[�̎��͂̃|���S�������J������
		MV1CollResultPolyDimTerminate(hitDim);
		return moveAfterPos;
	}
}

void MainScene::FadeInUpdate(const InputState& input)
{
	fadeValue_ = static_cast<int>(255 * static_cast<float>(fadeTimer_) / static_cast<float>(fade_interval));
	if (--fadeTimer_ <= 0)
	{
		fadeTimer_ = 0;
	}
}

void MainScene::NormalUpdate(const InputState& input)
{
	if (fadeTimer_ > 0)
	{
		// �t�F�[�h����
		FadeInUpdate(input);
	}

	// �e�N���X�̍X�V����
	pSkyDoom_->Update();
	pFieldManager_->Update();
	pPlayer_->Update(input);
	pEnemyManager_->Update();
	for (auto& shot : pShot_)
	{
		shot->Update();
	}
	pCamera_->Update(input);

	// �e�ƓG�̓����蔻��
	for (auto& shot : pShot_)
	{
		if (!shot->isExist()) continue;

		for (auto& enemies : pEnemyManager_->GetEnemies())
		{
			// DxLib�̊֐��𗘗p���ē����蔻����Ƃ�
			MV1_COLL_RESULT_POLY_DIM result;	// ������f�[�^
			result = MV1CollCheck_Capsule(enemies->GetModelHandle(), enemies->GetColFrameIndex(), shot->GetPos(), shot->GetLastPos(), shot->GetColRadius());

			if (result.HitNum > 0)		// 1���ȏ�̃|���S���Ɠ������Ă����烂�f���Ɠ������Ă��锻��
			{
				// ��������
				enemies->OnDamage(10);	// �G�Ƀ_���[�W
				shot->SetExsit(false);	// �G�ɓ��������e������
			}
			// �����蔻����̌�n��
			MV1CollResultPolyDimTerminate(result);
		}
	}

	// �G�ƃv���C���[�̓����蔻��
	for (auto& enemies : pEnemyManager_->GetEnemies())
	{
		float dist = VSize(VSub(enemies->GetPos(), pPlayer_->GetPos()));
		if (dist < (pPlayer_->GetColRadius() + enemies->GetColRadius()))
		{
			pPlayer_->OnDamage(1);
		}
	}
	if (pPlayer_->GetIsDead())
	{
		gameOverFadeTimer_++;
		gameOverUIfadeValue_ = static_cast<int>(255 * (static_cast<float>(gameOverFadeTimer_)) / static_cast<float>(game_over_fade_interval));
		if (gameOverFadeTimer_ >= 100)
		{
			gameOverFadeTimer_ = 100;
		}	 
	}

	// �V�[���؂�ւ�
	if (input.IsTriggered(InputType::next))
	{
		updateFunc_ = &MainScene::FadeOutUpdate;
	}
	if (input.IsTriggered(InputType::pause))
	{
		manager_.PushScene(new PauseScene(manager_));
	}
}

void MainScene::FadeOutUpdate(const InputState& input)
{
	fadeValue_ = static_cast<int>(255 * static_cast<float>(fadeTimer_) / static_cast<float>(fade_interval));
	if (++fadeTimer_ == fade_interval)
	{
		manager_.ChangeScene(new TitleScene(manager_));
		return;
	}
}