#include "MainScene.h"
#include "TitleScene.h"
#include "PauseScene.h"
#include "SceneManager.h"
#include "../EnemyManager.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Object/Player.h"
#include "../Object/Field.h"
#include "../Object/Shot.h"
#include "../Object/Enemy.h"

namespace
{
	// �t�F�[�h�̑��x
	constexpr int fade_interval = 60;

	// ��ʓ��ɑ��݂ł���e�̍ő吔
	constexpr int shot_max = 128;

	// �N���X�w�A�̈ʒu
	constexpr float reticle_pos_x = Game::screen_width / 2;
	constexpr float reticle_pos_y = Game::screen_height / 2;
}

MainScene::MainScene(SceneManager& manager) :
	Scene(manager),
	updateFunc_(&MainScene::FadeInUpdate),
	fadeTimer_(fade_interval),
	fadeValue_(255),
	shadowMap_(-1)
{
	pCamera_ = std::make_shared<Camera>();
	pPlayer_ = std::make_shared<Player>();
	pField_ = std::make_shared<Field>();
	pEnemyManager_ = std::make_shared<EnemyManager>();
	// �V���b�g�̐���
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
	pEnemyManager_->SetPlayer(pPlayer_);
	for (auto& shot : pShot_)
	{
		shot->SetPlayer(pPlayer_);
		shot->SetCamera(pCamera_);
		shot->Init();
	}
	pField_->Init();
	pPlayer_->Init();
	pEnemyManager_->Init();
	pCamera_->Init();

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

	// �N���X�w�A
	DrawLine(reticle_pos_x - 25, reticle_pos_y, reticle_pos_x + 25, reticle_pos_y, 0xffffff);	// ��
	DrawLine(reticle_pos_x, reticle_pos_y - 25, reticle_pos_x, reticle_pos_y + 25, 0xffffff);	// �c

	// �t�F�C�h
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeValue_);
	DrawBox(0, 0, Game::screen_width, Game::screen_height, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void MainScene::StartShot(VECTOR pos, VECTOR vec)
{
	for (auto& shot : pShot_)
	{
		if (shot->isExist())
		{
			continue;
		}
		shot->Start(pos, vec);
		break;
	}
}

float MainScene::GetReticlePosX() const
{
	return reticle_pos_x;
}

float MainScene::GetReticlePosY() const
{
	return reticle_pos_y;
}

void MainScene::FadeInUpdate(const InputState& input)
{
	fadeValue_ = static_cast<int>(255 * static_cast<float>(fadeTimer_) / static_cast<float>(fade_interval));
	if (--fadeTimer_ == 0)
	{
		updateFunc_ = &MainScene::NormalUpdate;
	}
}

void MainScene::NormalUpdate(const InputState& input)
{
	// �e�N���X�̍X�V����
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
			result = MV1CollCheck_Capsule(enemies->GetModelHandle(), enemies->GetColFrameIndex(), shot->GetPos(), shot->GetLastPos(), 48.0f);

			if (result.HitNum > 0)	// 1���ȏ�̃|���S���Ɠ������Ă����烂�f���Ɠ������Ă��锻��
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