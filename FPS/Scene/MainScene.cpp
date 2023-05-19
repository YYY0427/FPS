#include "MainScene.h"
#include "TitleScene.h"
#include "PauseScene.h"
#include "SceneManager.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Object/Player.h"
#include "../Object/Field.h"
#include "../Object/Shot.h"

namespace
{
	// �t�F�[�h�̑��x
	constexpr int fade_interval = 60;

	// ��ʓ��ɑ��݂ł���e�̍ő吔
	constexpr int shot_max = 128;

	// �N���X�w�A�̈ʒu
	constexpr int reticle_pos_x = Game::screen_width / 2;
	constexpr int reticle_pos_y = Game::screen_height / 2;
}

MainScene::MainScene(SceneManager& manager) :
	Scene(manager),
	updateFunc_(&MainScene::FadeInUpdate),
	fadeTimer_(fade_interval)
{
	pCamera_ = std::make_shared<Camera>();
	pPlayer_ = std::make_shared<Player>();
	pField_ = std::make_shared<Field>();
	// �V���b�g�̐���
	for (int i = 0; i < shot_max; i++)
	{
		pShot_.push_back(std::make_shared<Shot>());
		pShot_.back()->Init();
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
	pPlayer_->SetMainScene(static_cast<std::shared_ptr<MainScene>>(this));
	pCamera_->SetPlayer(pPlayer_);
	pPlayer_->SetCamera(pCamera_);
	for (auto& shot : pShot_)
	{
		shot->SetPlayer(pPlayer_);
	}
	pField_->Init();
	pPlayer_->Init();
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

	pCamera_->Draw();

	// �V���h�E�}�b�v�ւ̏�������
	ShadowMap_DrawSetup(shadowMap_);
	pField_->Draw();
	pPlayer_->Draw();
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
	for (auto& shot : pShot_)
	{
		shot->Draw();
	}
	// �`��I��
	SetUseShadowMap(0, -1);

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

int MainScene::GetReticlePosX() const
{
	return reticle_pos_x;
}

int MainScene::GetReticlePosY() const
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
	pField_->Update();
	pPlayer_->Update(input);
	for (auto& shot : pShot_)
	{
		shot->Update();
	}
	pCamera_->Update(input);

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