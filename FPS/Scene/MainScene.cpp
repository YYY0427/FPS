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
	pField_ = std::make_shared<Field>();
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
	pPlayer_->SetField(pField_);

	// 1�񂾂����f�������[�h���Ă�����g���ă��f���̕���
	int handle = pShot_[0]->LoadModel();
	for (auto& shot : pShot_)
	{
		shot->SetPlayer(pPlayer_);
		shot->SetCamera(pCamera_);
		shot->Init(handle);
	}
	pField_->Init();
	pPlayer_->Init();
	pEnemyManager_->Init();
	pCamera_->Init();
	pSkyDoom_->Init();

	for (auto& enemies : pEnemyManager_->GetEnemies())
	{
		enemies->SetPlayer(pPlayer_);
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
	pField_->Draw();
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
	pField_->Draw();			
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
	pField_->Update();
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