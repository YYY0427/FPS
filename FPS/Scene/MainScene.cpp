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
	// フェードの速度
	constexpr int fade_interval = 60;

	// 画面内に存在できる弾の最大数
	constexpr int shot_max = 128;

	// クロスヘアの位置
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
	// ショットの生成
	for (int i = 0; i < shot_max; i++)
	{
		pShot_.push_back(std::make_shared<Shot>());
	}
	Init();
}

MainScene::~MainScene()
{
	// シャドウマップの削除
	DeleteShadowMap(shadowMap_);
}

void MainScene::Init()
{
	// 初期化とポインタを渡す
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

	// シャドウマップの生成
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

	// シャドウマップへの書き込み
	ShadowMap_DrawSetup(shadowMap_);
	pField_->Draw();
	pPlayer_->Draw();
	pEnemyManager_->Draw();
	for (auto& shot : pShot_)
	{
		shot->Draw();
	}
	// 書き込み終了
	ShadowMap_DrawEnd();

	// シャドウマップを使用してモデルの描画
	SetUseShadowMap(0, shadowMap_);
	pField_->Draw();			
	pPlayer_->Draw();
	pEnemyManager_->Draw();
	for (auto& shot : pShot_)
	{
		shot->Draw();
	}
	// 描画終了
	SetUseShadowMap(0, -1);

	// 敵のHPの表示
	pEnemyManager_->DrawUI();

	// クロスヘア
	DrawLine(reticle_pos_x - 25, reticle_pos_y, reticle_pos_x + 25, reticle_pos_y, 0xffffff);	// 横
	DrawLine(reticle_pos_x, reticle_pos_y - 25, reticle_pos_x, reticle_pos_y + 25, 0xffffff);	// 縦

	// フェイド
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
	// 各クラスの更新処理
	pField_->Update();
	pPlayer_->Update(input);
	pEnemyManager_->Update();
	for (auto& shot : pShot_)
	{
		shot->Update();
	}
	pCamera_->Update(input);

	// 弾と敵の当たり判定
	for (auto& shot : pShot_)
	{
		if (!shot->isExist()) continue;

		for (auto& enemies : pEnemyManager_->GetEnemies())
		{
			// DxLibの関数を利用して当たり判定をとる
			MV1_COLL_RESULT_POLY_DIM result;	// あたりデータ
			result = MV1CollCheck_Capsule(enemies->GetModelHandle(), enemies->GetColFrameIndex(), shot->GetPos(), shot->GetLastPos(), 48.0f);

			if (result.HitNum > 0)	// 1枚以上のポリゴンと当たっていたらモデルと当たっている判定
			{
				// 当たった
				enemies->OnDamage(10);	// 敵にダメージ
				shot->SetExsit(false);	// 敵に当たった弾を消す
			}
			// 当たり判定情報の後始末
			MV1CollResultPolyDimTerminate(result);
		}
	}

	// 敵とプレイヤーの当たり判定
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