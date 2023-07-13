#include "MainScene.h"
#include "TitleScene.h"
#include "PauseScene.h"
#include "SceneManager.h"
#include "../StageManager.h"
#include "../Object/EnemyManager.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Object/Player.h"
#include "../Object/Shot.h"
#include "../Object/Enemy.h"
#include "../DrawFunctions.h"
#include "../Object/SkyDoom.h"
#include "../Object/Tower.h"
#include "../Collision.h"
#include "../EnemyShotFactory.h"
#include <cassert>
#include <stdlib.h>

namespace
{
	// フェードの速度
	constexpr int fade_interval = 30;

	// 画面内に存在できる弾の最大数
	constexpr int shot_max = 128;

	// クロスヘアの位置
	constexpr int reticle_pos_x = Game::screen_width / 2;
	constexpr int reticle_pos_y = Game::screen_height / 2;

	// ゲームオーバー時の表示文字のフェード速度
	constexpr int game_over_fade_interval = 60;
}

MainScene::MainScene(SceneManager& manager, StageManager* pStageManager) :
	Scene(manager),
	pStageManager_(pStageManager),
	updateFunc_(&MainScene::NormalUpdate),
	fadeTimer_(fade_interval),
	fadeValue_(255),
	shadowMap_(-1),
	gameOverUIhandle_(-1),
	gameOverUIFadeTimer_(0),
	gameClearUIhandle_(-1)
{
	pPlayer_ = std::make_shared<Player>(this);
	pTower_ = std::make_shared<Tower>(pStageManager);
	pCamera_ = std::make_shared<Camera>(pPlayer_, this);
	pSkyDoom_ = std::make_shared<SkyDoom>(pPlayer_);
	pCollision_ = std::make_shared<Collision>(pStageManager_, pTower_);
	pEnemyShotFactory_ = std::make_shared<EnemyShotFactory>(pPlayer_);
	pEnemyManager_ = std::make_shared<EnemyManager>(pPlayer_, pTower_, pCollision_, pEnemyShotFactory_);

	// 1回だけモデルをロードしてそれを使ってモデルの複製
	pShot_.push_back(std::make_shared<Shot>());
	int handle = pShot_[0]->LoadModel();
	for (int i = 0; i < shot_max - 1; i++)
	{
		pShot_.push_back(std::make_shared<Shot>(handle));
	}

	// 初期化とポインタを渡す
	pPlayer_->SetCamera(pCamera_);
	pPlayer_->SetCollision(pCollision_);
	pPlayer_->SetTower(pTower_);
	pTower_->SetEnemyManager(pEnemyManager_);
	pTower_->SetCollision(pCollision_);
	pCollision_->SetEnemyManager(pEnemyManager_);

	pStageManager_->Init();
	pEnemyManager_->Create(pTower_->GetCheckPoint());

	// 画像のロード
	gameOverUIhandle_ = my::MyLoadGraph("Data/UI/gameOver.png");
	gameClearUIhandle_ = my::MyLoadGraph("Data/UI/GameClear.png");

	// シャドウマップの生成
	shadowMap_ = MakeShadowMap(1024, 1024);
	SetShadowMapLightDirection(shadowMap_, GetLightDirection());
}

MainScene::~MainScene()
{
	// シャドウマップの削除
	DeleteShadowMap(shadowMap_);
}

void MainScene::Update(const InputState& input)
{
	(this->*updateFunc_)(input);
}

void MainScene::Draw()
{
	DrawString(0, 0, "MainScene", 0xffffff, true);

	pSkyDoom_->Draw();

	// シャドウマップへの書き込み
	{
		ShadowMap_DrawSetup(shadowMap_);
		pStageManager_->Draw();
		pPlayer_->Draw();
		pEnemyManager_->Draw();
		pTower_->Draw();
		for (auto& shot : pShot_)
		{
			shot->Draw();
		}
	}
	// 書き込み終了
	ShadowMap_DrawEnd();

	// シャドウマップを使用してモデルの描画
	{
		SetUseShadowMap(0, shadowMap_);
		pStageManager_->Draw();
		pPlayer_->Draw();
		pEnemyManager_->Draw();
		pTower_->Draw();
		pEnemyShotFactory_->Draw();
		for (auto& shot : pShot_)
		{
			shot->Draw();
		}
	}
	// 描画終了
	SetUseShadowMap(0, -1);

	// 敵のHPの表示
	pEnemyManager_->DrawUI();

	// ゲームオーバー時に表示開始
	if (isGameOver_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (gameOverUIfadeValue_ * 100) / 255);
		DrawGraph(0, 0, gameOverUIhandle_, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else
	{
		// クロスヘア
		DrawLine(reticle_pos_x - 25, reticle_pos_y, reticle_pos_x + 25, reticle_pos_y, 0xffffff);	// 横
		DrawLine(reticle_pos_x, reticle_pos_y - 25, reticle_pos_x, reticle_pos_y + 25, 0xffffff);	// 縦
	}

	// ゲームクリア時に表示
	if (pTower_->GetIsGoal() && !isGameOver_)
	{
		DrawGraph(0, 0, gameClearUIhandle_, true);
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
	// デバック表示
	DrawFormatString(10, 90, 0x000000, "playerX = %f", pPlayer_->GetPos().x);
	DrawFormatString(10, 120, 0x000000, "playerY = %f", pPlayer_->GetPos().y);
	DrawFormatString(10, 150, 0x000000, "playerZ = %f", pPlayer_->GetPos().z);
	pCamera_->Draw();
	DrawFormatString(10, 210, 0x000000, "shotNum = %d", cnt);
#endif

	// フェイド
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeValue_);
	DrawBox(0, 0, Game::screen_width, Game::screen_height, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void MainScene::StartPlayerShot(VECTOR pos, VECTOR vec)
{
	for (auto& shot : pShot_)
	{
		// 画面内にすでに存在していた場合ショットしない
		if (shot->isExist())
		{
			continue;
		}

		// 画面を揺らす度合いをセット
		pCamera_->SetQuake(10, VGet(0, 2, 0));

		// ショットスタート
		shot->Start(pos, vec, pPlayer_, pCamera_);
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

void MainScene::PlayerFallFade()
{
	fadeValue_ = static_cast<int>(255 * static_cast<float>(fadeTimer_) / static_cast<float>(fade_interval));
	if (++fadeTimer_ == fade_interval)
	{
		pPlayer_->SetRespawn();
	}
}

void MainScene::FadeInUpdate()
{
	fadeValue_ = static_cast<int>(255 * static_cast<float>(fadeTimer_) / static_cast<float>(fade_interval));
	if (--fadeTimer_ <= 0)
	{
		fadeTimer_ = 0;
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

void MainScene::NormalUpdate(const InputState& input)
{
	if (fadeTimer_ > 0 && !pPlayer_->IsFall())
	{
		// フェード処理
		FadeInUpdate();
	}

	// 各クラスの更新処理
	{
		pSkyDoom_->Update();
		pStageManager_->Update();
		pPlayer_->Update(input);
		pEnemyManager_->Update();
		pTower_->Update();
		pEnemyShotFactory_->Update();
		for (auto& shot : pShot_)
		{
			shot->Update();
		}
		pCamera_->Update(input);
	}
	
	// TODO:Collisionクラスに移す
	{
		// 弾と敵の当たり判定
		for (auto& shot : pShot_)
		{
			if (!shot->isExist()) continue;

			for (auto& enemies : pEnemyManager_->GetEnemies())
			{
				// DxLibの関数を利用して当たり判定をとる
				MV1_COLL_RESULT_POLY_DIM result;	// あたりデータ
				result = MV1CollCheck_Capsule(enemies->GetModelHandle(), enemies->GetColFrameIndex(), shot->GetPos(), shot->GetLastPos(), shot->GetColRadius());

				if (result.HitNum > 0)		// 1枚以上のポリゴンと当たっていたらモデルと当たっている判定
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

		// 敵とタワーの当たり判定
		for (auto& enemies : pEnemyManager_->GetEnemies())
		{
			// 死んでいたら当たり判定を行わない
			if (enemies->GetDead()) continue;

			// DxLibの関数を利用して当たり判定をとる
			MV1_COLL_RESULT_POLY_DIM result;	// あたりデータ
		//	result = MV1CollCheck_Sphere(enemies->GetModelHandle(), enemies->GetColFrameIndex(), pTower_->GetPos(), pTower_->GetColRadius());
			result = MV1CollCheck_Sphere(pTower_->GetModelHandle(), -1, enemies->GetPos(), enemies->GetColRadius() + 30.0f);

			if (result.HitNum > 0)		// 1枚以上のポリゴンと当たっていたらモデルと当たっている判定
			{
				// 当たった
				pTower_->OnDamage(1);	// タワーにダメージ
			}
			// 当たり判定情報の後始末
			MV1CollResultPolyDimTerminate(result);
		}
	}

	// ゲームオーバー演出開始
	if (pPlayer_->GetIsDead() || pTower_->GetIsDead())
	{
		gameOverUIFadeTimer_++;
		gameOverUIfadeValue_ = static_cast<int>(255 * (static_cast<float>(gameOverUIFadeTimer_)) / static_cast<float>(game_over_fade_interval));
		if (gameOverUIFadeTimer_ >= 100)
		{
			gameOverUIFadeTimer_ = 100;
		}
		isGameOver_ = true;
	}
#ifdef _DEBUG
	// シーン切り替え
	if (input.IsTriggered(InputType::next))
	{
		updateFunc_ = &MainScene::FadeOutUpdate;
	}
#endif
	if (input.IsTriggered(InputType::pause))
	{
		manager_.PushScene(new PauseScene(manager_));
	}
}