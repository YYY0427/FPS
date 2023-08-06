#include "MainScene.h"
#include "TitleScene.h"
#include "PauseScene.h"
#include "SceneManager.h"
#include "../StageManager.h"
#include "../StageBase.h"
#include "../Object/EnemyManager.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Object/Player.h"
#include "../Object/Shot.h"
#include "../Object/EnemyBase.h"
#include "../DrawFunctions.h"
#include "../Object/SkyDoom.h"
#include "../Object/Tower.h"
#include "../Collision.h"
#include "../EnemyShotFactory.h"
#include "../EnemyShot.h"
#include "../ObstacleManager.h"
#include "../Obstacle.h"
#include "../Bom.h"
#include "../BomManager.h"
#include "../UI.h"
#include "../ThreeDimensionEffectManager.h"
#include "../SoundManager.h"
#include <cassert>
#include <stdlib.h>

namespace
{
	// フェードの速度
	constexpr int fade_interval = 100;

	// 画面内に存在できる弾の最大数
	constexpr int shot_max = 128;

	// クロスヘアの位置
	constexpr int reticle_pos_x = Game::screen_width / 2;
	constexpr int reticle_pos_y = Game::screen_height / 2;

	// クロスヘアの太さ
	constexpr int reticle_thickness = 1;

	// クロスヘアの長さ
	constexpr int reticle_length = 25;

	// クロスヘアが中心からどれだけ離れているか
	constexpr int reticle_distance_from_center = 10;

	// ゲームオーバー時の表示文字のフェード速度
	constexpr int game_over_fade_interval = 60;

	// ゲームクリア時の表示文字のフェード速度
	constexpr int game_clear_fade_interval = 60;

	// プレイヤーダメージのUIのフェード速度
	constexpr int player_damage_ui_fade_interval = 60;

	constexpr int game_clear_stamp_fade_interval = 60;

	// ゲームクリア時のエフェクトの位置
	constexpr VECTOR gameclear_effect_pos{ -4434, -342, -6460 };

	// プレイヤーショットのダメージ
	constexpr int player_shot_damage = 6;

	// ボムのダメージ
	constexpr int player_bom_shot_damage = 50;
}

MainScene::MainScene(SceneManager& manager, StageManager* pStageManager) :
	Scene(manager),
	pStageManager_(pStageManager),
	updateFunc_(&MainScene::UserReactionWaitUpdate),
	fadeTimer_(fade_interval),
	fadeValue_(255),
	shadowMap_(-1),
	gameOverUIhandle_(-1),
	gameClearUIhandle_(-1),
	gameClearImgExRate_(3),
	playerDamageUIFadeTimer_(0),
	isPass_(false),
	isGameStop_(true),
	questImgExRate_(4.0),
	questUIfadeTimer_(0)
{
	pBomManager_ = std::make_shared<BomManager>();
	pObstacleManager_ = std::make_shared<ObstacleManager>();
	pUI_ = std::make_shared<UI>();
	pEnemyManager_ = std::make_shared<EnemyManager>();
	pPlayer_ = std::make_shared<Player>(this);
	pTower_ = std::make_shared<Tower>(pStageManager, pObstacleManager_);
	pSkyDoom_ = std::make_shared<SkyDoom>(pPlayer_);
	pCamera_ = std::make_shared<Camera>(pPlayer_, this);
	pEnemyShotFactory_ = std::make_shared<EnemyShotFactory>(pPlayer_, pTower_);
	pCollision_ = std::make_shared<Collision>(pStageManager_, pTower_, pEnemyManager_, pObstacleManager_, pPlayer_);

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
	pPlayer_->SetBomManager(pBomManager_);
	pTower_->SetEnemyManager(pEnemyManager_);
	pTower_->SetCollision(pCollision_);

	pStageManager_->Init();
	pEnemyManager_->FirstCreate(pTower_->GetCheckPoint(), pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, this);

	// 画像のロード
	gameOverUIhandle_ = my::MyLoadGraph("Data/UI/gameOver.png");
	gameClearUIhandle_ = my::MyLoadGraph("Data/UI/GameClear.png");
	towerIconHandle_ = my::MyLoadGraph("Data/UI/balloonIcon1.png");
	playerIconHandle_ = my::MyLoadGraph("Data/UI/playerIcon1.png");
	gunUIhandle_ = my::MyLoadGraph("Data/UI/gunUI.png");
	bomUIhandle_ = my::MyLoadGraph("Data/UI/bomUI2.png");
	infinityHandle_ = my::MyLoadGraph("Data/UI/infinity.png");
	completeHandle_ = my::MyLoadGraph("Data/UI/complete.png");
	playerDamageUIHandle_ = my::MyLoadGraph("Data/UI/damageUI2.png");
	questUIHandle_ = my::MyLoadGraph("Data/UI/quest.png");

	// フォントのロード
	bulletCounFontHandle_ = CreateFontToHandle("ニコカv2", 40, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	towerNameFontHandle_ = CreateFontToHandle("ニコカv2", 30, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	towerNameShadowFontHandle_ = CreateFontToHandle("ニコカv2", 31, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	playerNameFontHandle_ = CreateFontToHandle("ニコカv2", 40, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	playerNameShadowFontHandle_ = CreateFontToHandle("ニコカv2", 41, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	gameClearFontHandle_ = CreateFontToHandle("ニコカv2", 120, 3, DX_FONTTYPE_ANTIALIASING_4X4);
	
	// シャドウマップの生成
	shadowMap_ = MakeShadowMap(1024, 1024);
	SetShadowMapLightDirection(shadowMap_, GetLightDirection());

	auto& soundManager = SoundManager::GetInstance();

	//↓ここ勝手に変更下でby大島
	soundManager.PlayMusic("bgm");
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

void MainScene::UserReactionWaitUpdate(const InputState& input)
{
	// 各クラスの更新処理
	{
		pSkyDoom_->Update();
		pStageManager_->Update();
		pPlayer_->Update(input);
		pEnemyManager_->Update();
		pTower_->Update(isGameStop_);
		pObstacleManager_->Update();
		pEnemyShotFactory_->Update();
		pBomManager_->Update();
		for (auto& shot : pShot_)
		{
			shot->Update();
		}
		pCamera_->Update(input);
	}
	auto& soundManager = SoundManager::GetInstance();
	static bool isPass = false;
	static bool isNext = false;
	static int fadeTimer = 0;
	if (fadeTimer_ > 0 && !pPlayer_->IsFall())
	{
		// フェード処理
		FadeInUpdate();
	}
	else if (fadeTimer_ <= 0)
	{
		fadeTimer++;
		questUIfadeValue_ = static_cast<int>(255 * (static_cast<float>(fadeTimer)) / static_cast<float>(game_over_fade_interval));
		if (fadeTimer >= game_over_fade_interval)
		{
			fadeTimer = game_over_fade_interval;
		}

		questImgExRate_ = questImgExRate_ - 0.08;
		if (questImgExRate_ <= 1.0)
		{
			questImgExRate_ = 1.0;
			if (!isPass)
			{
				soundManager.Play("don");
				isPass = true;
			}
			if (input.IsTriggered(InputType::next) && isPass)
			{
				soundManager.Play("book");
				isNext = true;
			}
			if (isNext)
			{
				static int timer = 0;
				if (timer++ > 10)
				{
					isGameStop_ = false;
					updateFunc_ = &MainScene::NormalUpdate;
					timer = 0;
				}
			}
		}
	}
}

void MainScene::NormalUpdate(const InputState& input)
{
	auto& effectManager = ThreeDimensionEffectManager::GetInstance();
	auto& soundManager = SoundManager::GetInstance();

	if (reticleEffectDisplayTime++ > 45)
	{
		isHit_ = false;
		reticleEffectDisplayTime = 0;
	}

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
		pTower_->Update(isGameStop_);
		pEnemyManager_->Update();
		pObstacleManager_->Update();
		pEnemyShotFactory_->Update();
		pBomManager_->Update();
		for (auto& shot : pShot_)
		{
			shot->Update();
		}
		pCamera_->Update(input);
	}

	pEnemyManager_->AdditionCreate(pTower_->GetCheckPoint(), pPlayer_, pTower_, pCollision_, pEnemyShotFactory_, this);

	// 当たり判定
	CollisionUpdate();

	// プレイヤーがダメージを受けた時の演出
	playerDamageUIFadeTimer_--;
	playerDamageUIFadeValue_ = static_cast<int>(255 * (static_cast<float>(playerDamageUIFadeTimer_)) / static_cast<float>(player_damage_ui_fade_interval));
	if (playerDamageUIFadeTimer_ <= 0)
	{
		playerDamageUIFadeTimer_ = 0;
	}

	// ゲームクリア演出開始
	if (pTower_->GetIsGoal() && !isGameOver_)
	{
		static int gameClearTimer = 0;

		// 元々流れていたBGMを止める
		soundManager.StopSelectMusic("bgm");

		// 花火演出
		if (!effectManager.IsEffectPlaying("gameClear"))
		{
			// 一度だけ
			if (!isPass_)
			{
				soundManager.Play("gameClear");
			}
			soundManager.StopSelectMusic("hanabi2");
			soundManager.Play3D("hanabi", pTower_->GetPos(), 10000, false);
			effectManager.PlayEffect("gameClear", gameclear_effect_pos, 100.0f, 1.0f);
		}
		else
		{
			if (!soundManager.CheckMusic("hanabi") && !soundManager.CheckMusic("hanabi2"))
			{
				soundManager.Play3D("hanabi2", pTower_->GetPos(), 10000, false);
			}
		}

		// COMPLETEスタンプ演出
		if (gameClearTimer++ > 280)
		{
			static int fadeTimer = 0;
			fadeTimer++;
			gameClearUIFadeValue_ = static_cast<int>(255 * (static_cast<float>(fadeTimer)) / static_cast<float>(game_clear_stamp_fade_interval));
			if (fadeTimer >= game_clear_stamp_fade_interval)
			{
				fadeTimer = game_clear_stamp_fade_interval;
			}
			if (fadeTimer >= 30)
			{
				gameClearImgExRate_ = gameClearImgExRate_ - 0.1;
				if (gameClearImgExRate_ <= 1.0)
				{
					gameClearImgExRate_ = 1.0;
					if (!isPass_)
					{
						soundManager.Play("don");
						isPass_ = true;
					}
				}
			}
		}
	}

	// ゲームオーバー演出開始
	if (pPlayer_->GetIsDead() || pTower_->GetIsDead())
	{
		static int timer = 0;
		timer++;
		gameOverUIfadeValue_ = static_cast<int>(255 * (static_cast<float>(timer)) / static_cast<float>(game_over_fade_interval));
		if (timer >= game_over_fade_interval)
		{
			timer = game_over_fade_interval;
		}
		isGameOver_ = true;
	}

#if false
	// シーン切り替え
	if (input.IsTriggered(InputType::next))
	{
		soundManager.StopMusic();
		updateFunc_ = &MainScene::FadeOutUpdate;
	}
#endif
	// シーン切り替え
	if (isPass_)
	{
		static int timer = 0;
		if (timer++ > 180)
		{
			soundManager.StopMusic();
			effectManager.EffectAllStop();
			updateFunc_ = &MainScene::FadeOutUpdate;
		}
	}
	if (input.IsTriggered(InputType::pause))
	{
		manager_.PushScene(new PauseScene(manager_));
	}
	if (input.IsTriggered(InputType::up))
	{
	//	isGameStop_ = true;
	}
}

void MainScene::Draw()
{
	DrawString(0, 0, "MainScene", 0xffffff, true);

	pSkyDoom_->Draw();

	// シャドウマップへの書き込み
	{
		ShadowMap_DrawSetup(shadowMap_);
		pStageManager_->Draw();
		pObstacleManager_->Draw();
		pPlayer_->Draw();
		pEnemyManager_->Draw();
		pTower_->Draw();
		pEnemyShotFactory_->Draw();
		pBomManager_->Draw();
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
		pObstacleManager_->Draw();
		pPlayer_->Draw();
		pEnemyManager_->Draw();
		pTower_->Draw();
		pEnemyShotFactory_->Draw();
		pBomManager_->Draw();
		for (auto& shot : pShot_)
		{
			shot->Draw();
		}
	}
	// 描画終了
	SetUseShadowMap(0, -1);

	// プレイヤーがダメージを受けた
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, playerDamageUIFadeValue_);
	DrawGraph(0, 0, playerDamageUIHandle_, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	if (!isGameStop_)
	{
		// HPの表示
		{
			for (auto& obj : pObstacleManager_->GetObstacles())
			{
				pUI_->DrawEnemyHpBar(pPlayer_, obj->GetPos(), obj->GetModelHandle(), obj->GetHP().hp_, obj->GetHP().maxHp_, "Stairs", obj->GetHP().hpUIDrawY_);
			}
			for (auto& enemies : pEnemyManager_->GetEnemies())
			{
				pUI_->DrawEnemyHpBar(pPlayer_, enemies->GetPos(), enemies->GetModelHandle(), enemies->GetHP().hp_, enemies->GetHP().maxHp_, "Head3_end", enemies->GetHP().hpUIDrawY_);
			}
			pUI_->DrawAllyHpBar(pPlayer_->GetHP(), pPlayer_->GetMaxHP(), 450, 900);
			pUI_->DrawAllyHpBar(pTower_->GetHP(), pTower_->GetMaxHP(), Game::screen_width / 2, 100);
			DrawRotaGraph(Game::screen_width / 2 - 320, 115, 1.0f, 0.0f, towerIconHandle_, true);
			DrawRotaGraph(120, 920, 1.0f, 0.0f, playerIconHandle_, true);

			DrawStringToHandle(Game::screen_width / 2 - 20, 65, "気球", 0x000000, towerNameShadowFontHandle_);
			DrawStringToHandle(Game::screen_width / 2 - 20, 65, "気球", 0xffffff, towerNameFontHandle_);

			DrawStringToHandle(450 - 35, 855, "あなた", 0x000000, playerNameFontHandle_);
			DrawStringToHandle(450 - 35, 855, "あなた", 0xffffff, playerNameShadowFontHandle_);
		}

		// 銃のUI
		DrawRoundRectAA(Game::screen_width - 500, Game::screen_height - 220, Game::screen_width - 50, Game::screen_height - 80, 1.0f, 1.0f, 4, 0xffffff, false, 2);
		DrawLine(Game::screen_width - 140, Game::screen_height - 180, Game::screen_width - 140, Game::screen_height - 120, 0xffffff, 2);
		DrawRotaGraph(Game::screen_width - 95, Game::screen_height - 150, 1.0, 0.0f, infinityHandle_, true);
		DrawRotaGraph(Game::screen_width - 350, Game::screen_height - 150, 1.0f, 0.0f, gunUIhandle_, true);

		// 爆弾のUI
		if (pPlayer_->GetIsUseBom())
		{
			DrawRoundRectAA(Game::screen_width - 720, Game::screen_height - 220, Game::screen_width - 580, Game::screen_height - 80, 3.0f, 3.0f, 4, 0xffffff, false, 2);
			DrawRotaGraph(Game::screen_width - 650, Game::screen_height - 150, 1.0f, 0.0f, bomUIhandle_, true);
		}
		else
		{
			DrawRoundRectAA(Game::screen_width - 720, Game::screen_height - 220, Game::screen_width - 580, Game::screen_height - 80, 3.0f, 3.0f, 4, 0xffffff, true, 2);
			DrawFormatStringToHandle(Game::screen_width - 660, Game::screen_height - 175, 0x000000, bulletCounFontHandle_, "%d", (pPlayer_->GetBomFrameCount() + 60) / 60);
		}
	}

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
		DrawBox(reticle_pos_x - reticle_length, reticle_pos_y - reticle_thickness, reticle_pos_x - reticle_distance_from_center, reticle_pos_y + reticle_thickness, 0xffffff, true);
		DrawBox(reticle_pos_x + reticle_distance_from_center, reticle_pos_y - reticle_thickness, reticle_pos_x + reticle_length, reticle_pos_y + reticle_thickness, 0xffffff, true);
		DrawBox(reticle_pos_x - reticle_thickness, reticle_pos_y - reticle_length, reticle_pos_x + reticle_thickness, reticle_pos_y - 10, 0xffffff, true);
		DrawBox(reticle_pos_x - reticle_thickness, reticle_pos_y + reticle_distance_from_center, reticle_pos_x + reticle_thickness, reticle_pos_y + 25, 0xffffff, true);

		if (isHit_)
		{
			DrawLine(reticle_pos_x - 15, reticle_pos_y - 15, reticle_pos_x + 15, reticle_pos_y + 15, 0xff0000, 2);
			DrawLine(reticle_pos_x - 15, reticle_pos_y + 15, reticle_pos_x + 15, reticle_pos_y - 15, 0xff0000, 2);	
		}	
	}

	if (isGameStop_ && fadeTimer_ <= 0)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, questUIfadeValue_);
		DrawRotaGraph(Game::screen_width / 2, Game::screen_height / 2, questImgExRate_, 0.0, questUIHandle_, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
		
	// ゲームクリア時に表示
	if (pTower_->GetIsGoal() && !isGameOver_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, gameClearUIFadeValue_);
		DrawRotaGraph(950, 550, gameClearImgExRate_, -15.0f * DX_PI_F / 180.0f, completeHandle_, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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

void MainScene::CollisionUpdate()
{
	auto& effectManager = ThreeDimensionEffectManager::GetInstance();
	auto& soundManager = SoundManager::GetInstance();
	for (auto& shot : pShot_)
	{
		if (!shot->isExist()) continue;

		// プレイヤーの弾と敵の当たり判定
		for (auto& enemies : pEnemyManager_->GetEnemies())
		{
			if (pCollision_->ModelAndCapsuleCollision(enemies->GetModelHandle(), enemies->GetColFrameIndex(), shot->GetPos(), shot->GetLastPos(), shot->GetColRadius()))
			{
				if (!pTower_->GetIsGoal())
					enemies->OnDamage(player_shot_damage);
				shot->SetEnabled(false);
				isHit_ = true;
				auto temp = pCollision_->GetCollisionResult().Dim->Position;
				effectManager.PlayEffect("hit", VGet(temp->x, temp->y, temp->z), 50.0f, 2.0f);
			}
		}
		// 障害物とプレイヤーショットの当たり判定
		for (auto& obj : pObstacleManager_->GetObstacles())
		{
			if (pCollision_->ModelAndSphereCollision(shot->GetModelHandle(), -1, obj->GetPos(), obj->GetNormalCollsionRadius()))
			{
				if (!pTower_->GetIsGoal())
					obj->OnDamage(player_shot_damage);
				shot->SetEnabled(false);	// 敵に当たった弾を消す
				isHit_ = true;
				auto temp = pCollision_->GetCollisionResult().Dim->Position;
				effectManager.PlayEffect("hit", VGet(temp->x, temp->y, temp->z), 50.0f, 1.0f);
			}
		}
		// プレイヤーショットとステージの当たり判定
		if (pCollision_->ModelAndCapsuleCollision(pStageManager_->GetStages()->GetModelHandle(), -1, shot->GetPos(), shot->GetLastPos(), shot->GetColRadius()))
		{
			// 当たった
			shot->SetEnabled(false);
		}
	}
	for (auto& boms : pBomManager_->GetBoms())
	{
		bool isBomHit = false;
		// ボムと障害物の当たり判定
		for (auto& obs : pObstacleManager_->GetObstacles())
		{
			if (pCollision_->SpheresColision(obs->GetPos(), boms->GetPos(), obs->GetNormalCollsionRadius(), boms->GetCollisionRadius()))
			{
				if (!pTower_->GetIsGoal())
					obs->OnDamage(player_bom_shot_damage);
				isHit_ = true;
				isBomHit = true;
			}
		}
		// 敵とボムの当たり判定
		for (auto& enemies : pEnemyManager_->GetEnemies())
		{
			bool isBoss = false;
			if (enemies->GetEnemyType() == EnemyBase::enemyBos) isBoss = true;
			if (pCollision_->ModelAndSphereCollision(enemies->GetModelHandle(), enemies->GetColFrameIndex(), boms->GetPos(), boms->GetCollisionRadius()))
			{
				isHit_ = true;
				isBomHit = true;
				if (!pTower_->GetIsGoal() && !isBoss)
				{
					enemies->OnDamage(enemies->GetHP().maxHp_ - 1);
				}
				else if (!pTower_->GetIsGoal() && isBoss)
				{
					enemies->OnDamage(enemies->GetHP().maxHp_ / 3);
				}
			}
		}
		// ボムとステージの当たり判定
		if (pCollision_->ModelAndSphereCollision(pStageManager_->GetStages()->GetModelHandle(), -1, boms->GetPos(), boms->GetCollisionRadius()))
		{
			// 当たった
			isBomHit = true;
		}

		if (isBomHit)
		{
			boms->StartExplosion();
		}
	}
	for (auto& bullets : pEnemyShotFactory_->GetBullets())
	{
		// 敵の弾とプレイヤーの当たり判定
		if (pCollision_->ModelAndCapsuleCollision(pPlayer_->GetHandle(), -1, bullets->GetPos(), bullets->GetLastPos(), bullets->GetColRadius()))
		{
			// 当たった
			if (!pTower_->GetIsGoal())
				pPlayer_->OnDamage(2);
			bullets->SetIsEnabled(false);
			playerDamageUIFadeTimer_ = player_damage_ui_fade_interval;
			if (!soundManager.CheckMusic("playerDamage"))
				soundManager.Play("playerDamage");
		}
		// 敵の弾とタワーの当たり判定
		{
			MV1_COLL_RESULT_POLY_DIM result = MV1CollCheck_Capsule(pTower_->GetModelHandle(), pTower_->GetCollisionFrameIndex(), bullets->GetPos(), bullets->GetLastPos(), bullets->GetColRadius());
			if (result.HitNum > 0)
			{
				// 当たった
				effectManager.PlayEffect("hit", VGet(result.Dim->Position->x, result.Dim->Position->y, result.Dim->Position->z), 100.0f, 1.0f);
				if (!pTower_->GetIsGoal())
					pTower_->OnDamage(2);
				bullets->SetIsEnabled(false);
				soundManager.Play3D("hit", pTower_->GetPos(), 5000, false);
			}
			// 当たり判定情報の後始末
			MV1CollResultPolyDimTerminate(result);
		}
		// エネミーショットとステージの当たり判定
		if (pCollision_->ModelAndCapsuleCollision(pStageManager_->GetStages()->GetModelHandle(), -1, bullets->GetPos(), bullets->GetLastPos(), bullets->GetColRadius()))
		{
			bullets->SetIsEnabled(false);
		}
	}
	for (auto& enemies : pEnemyManager_->GetEnemies())
	{
		bool isBoss = false;
		// 敵の種別によって当たり判定を行わない
		if (enemies->GetEnemyType() == EnemyBase::EnemyType::bee) continue;
		// 攻撃をしていなかったら当たり判定を行わない
		if (!enemies->GetIsAttak()) continue;
		// 敵のHPがなかったら判定を行わない
		if (enemies->GetHP().hp_ <= 0) continue;
		if (enemies->GetEnemyType() == EnemyBase::EnemyType::enemyBos) isBoss = true;

		// 敵とプレイヤーの当たり判定
		if (pCollision_->SpheresColision(enemies->GetPos(), pPlayer_->GetPos(), enemies->GetCollisionRadius(), pPlayer_->GetCollisionRadius()))
		{
			if (!pTower_->GetIsGoal() && !isBoss)
			{
				pPlayer_->OnDamage(5);
			}
			else if (!pTower_->GetIsGoal() && isBoss)
			{
				pPlayer_->OnDamage(20);
			}
			playerDamageUIFadeTimer_ = player_damage_ui_fade_interval;
			if (!soundManager.CheckMusic("playerDamage"))
				soundManager.Play("playerDamage");
		}
		// 敵とタワーの当たり判定
		{
			// 敵の種別によって当たり判定を行わない
			if (enemies->GetEnemyType() == EnemyBase::EnemyType::bee) continue;
			// 攻撃をしていなかったら当たり判定を行わない
			if (!enemies->GetIsAttak()) continue;
			// 敵のHPがなかったら判定を行わない
			if (enemies->GetHP().hp_ <= 0) continue;
			if (enemies->GetEnemyType() == EnemyBase::EnemyType::enemyBos) isBoss = true;

			if (pCollision_->ModelAndSphereCollision(pTower_->GetModelHandle(), pTower_->GetCollisionFrameIndex(), enemies->GetPos(), enemies->GetCollisionRadius() + 30.0f))
			{
				// 当たった
				auto temp = pCollision_->GetCollisionResult().Dim->Position;
				effectManager.PlayEffect("hit", VGet(temp->x, temp->y, temp->z), 100.0f, 1.0f);
				soundManager.Play3D("hit", pTower_->GetPos(), 5000, false);
				if (!pTower_->GetIsGoal() && !isBoss && enemies->GetIsAttak())
				{
					pTower_->OnDamage(5);
				}
				else if(!pTower_->GetIsGoal() && isBoss && enemies->GetIsAttak())
				{
					pTower_->OnDamage(20);
				}
			}
		}
	}
}