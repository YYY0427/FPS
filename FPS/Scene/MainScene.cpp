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
	// シャドウマップの削除
	DeleteShadowMap(shadowMap_);
}

void MainScene::Init()
{
	// 初期化とポインタを渡す
	pPlayer_->SetMainScene(static_cast<std::shared_ptr<MainScene>>(this));
	pCamera_->SetPlayer(pPlayer_);
	pPlayer_->SetCamera(pCamera_);
	pSkyDoom_->SetPlayer(pPlayer_);
	pPlayer_->SetField(pFieldManager_->GetField());

	// 1回だけモデルをロードしてそれを使ってモデルの複製
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

	// 画像のロード
	gameOverUIhandle_ = my::MyLoadGraph("Data/Texture/youdead.png");

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

	pSkyDoom_->Draw();

	// シャドウマップへの書き込み
	ShadowMap_DrawSetup(shadowMap_);
	pFieldManager_->Draw();
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
	pFieldManager_->Draw();
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

	// プレイヤーが死んだら表示開始
	if (pPlayer_->GetIsDead())
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (gameOverUIfadeValue_ * 100) / 255);
		DrawGraph(0,  0, gameOverUIhandle_, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	// プレイヤーが生きているときのみクロスヘアを表示
	else
	{
		// クロスヘア
		DrawLine(reticle_pos_x - 25, reticle_pos_y, reticle_pos_x + 25, reticle_pos_y, 0xffffff);	// 横
		DrawLine(reticle_pos_x, reticle_pos_y - 25, reticle_pos_x, reticle_pos_y + 25, 0xffffff);	// 縦
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

void MainScene::StartShot(VECTOR pos, VECTOR vec)
{
	for (auto& shot : pShot_)
	{
		// 画面内にすでに存在していた場合ショットしない
		if (shot->isExist())
		{
			continue;
		}

		// 画面を揺らす度合いをセット
		pCamera_->SetQuake(10, VGet(3, 0, 0));

		// ショットスタート
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

		// 初期化
		bool hitFlag = false;
		int yukaNum = 0;
		int kabeNum = 0;

		// モデルの最大頂点座標と最小頂点座標の取得
		MV1_REF_POLYGONLIST refPoly = MV1GetReferenceMesh(modelHandle, -1, true);

		// 移動前のプレイヤーの座標
		VECTOR oldPos = pos;

		// 移動後のプレイヤーの座標
		VECTOR moveAfterPos = VAdd(oldPos, vec);

		// プレイヤーとフィールドの当たり判定(何枚のポリゴンと当たっているか)
		MV1_COLL_RESULT_POLY_DIM hitDim = MV1CollCheck_Sphere(field->GetModelHandle(), -1, oldPos, (refPoly.MaxPosition.y + 50.0f) + VSize(vec));

		// 検出されたポリゴンの数だけ繰り返し
		for (int i = 0; i < hitDim.HitNum; i++)
		{
			// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
			if (hitDim.Dim[i].Normal.y < 0.000001f && hitDim.Dim[i].Normal.y > -0.000001f)
			{
				// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
				if (hitDim.Dim[i].Position[0].y > oldPos.y + 1.0f ||
					hitDim.Dim[i].Position[1].y > oldPos.y + 1.0f ||
					hitDim.Dim[i].Position[2].y > oldPos.y + 1.0f)
				{
					// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
					if (kabeNum < 2048)
					{
						// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
						kabe_[kabeNum] = &hitDim.Dim[i];

						// 壁ポリゴンの数を加算する
						kabeNum++;
					}
				}
			}
			else
			{
				// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
				if (yukaNum < 2048)
				{
					// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
					yuka_[yukaNum] = &hitDim.Dim[i];

					// 床ポリゴンの数を加算する
					yukaNum++;
				}
			}
		}

		// 壁ポリゴンとの当たり判定処理
		if (kabeNum != 0)
		{
			// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
			hitFlag = false;

			int i, j = 0;

			// 移動したかどうかで処理を分岐
			if (isMove)
			{
				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < kabeNum; i++)
				{
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					poly = kabe_[i];

					// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == false) continue;

					// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
					hitFlag = true;

					// 壁に当たったら壁に遮られない移動成分分だけ移動する
					{
						VECTOR SlideVec;	// プレイヤーをスライドさせるベクトル

						// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
						SlideVec = VCross(vec, poly->Normal);

						// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
						// 元の移動成分から壁方向の移動成分を抜いたベクトル
						SlideVec = VCross(poly->Normal, SlideVec);

						// それを移動前の座標に足したものを新たな座標とする
						moveAfterPos = VAdd(oldPos, SlideVec);
					}

					// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
					for (j = 0; j < kabeNum; j++)
					{
						// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
						poly = kabe_[j];

						// 当たっていたらループから抜ける
						if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
					}

					// j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
					// 壁に当たったフラグを倒した上でループから抜ける
					if (j == kabeNum)
					{
						hitFlag = false;
						break;
					}
				}
			}
			else
			{
				// 移動していない場合の処理

				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < kabeNum; i++)
				{
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					poly = kabe_[i];

					// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
					if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE)
					{
						hitFlag = true;
						break;
					}
				}
			}

			// 壁に当たっていたら壁から押し出す処理を行う
			if (hitFlag)
			{
				int k, i, j;

				// 壁からの押し出し処理を試みる最大数だけ繰り返し
				for (k = 0; k < 16; k++)
				{
					// 壁ポリゴンの数だけ繰り返し
					for (i = 0; i < kabeNum; i++)
					{
						// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
						poly = kabe_[i];

						// プレイヤーと当たっているかを判定
						if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == FALSE) continue;

						// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
						moveAfterPos = VAdd(moveAfterPos, VScale(poly->Normal, 5.0f));

						// 移動した上で壁ポリゴンと接触しているかどうかを判定
						for (j = 0; j < kabeNum; j++)
						{
							// 当たっていたらループを抜ける
							poly = kabe_[j];
							if (HitCheck_Capsule_Triangle(moveAfterPos, VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
						}

						// 全てのポリゴンと当たっていなかったらここでループ終了
						if (j == kabeNum) break;
					}

					// i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
					if (i != kabeNum) break;
				}
			}
		}
		// 床ポリゴンとの当たり判定
		if (yukaNum != 0)
		{
			float MaxY;

			// 床ポリゴンに当たったかどうかのフラグを倒しておく
			hitFlag = false;

			// 一番高い床ポリゴンにぶつける為の判定用変数を初期化
			MaxY = 0.0f;

			// 床ポリゴンの数だけ繰り返し
			for (int i = 0; i < yukaNum; i++)
			{
				// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				poly = yuka_[i];

				// ジャンプ中かどうかで処理を分岐
				if (isJump)
				{
					// ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
					lineRes = HitCheck_Line_Triangle(VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), VAdd(moveAfterPos, VGet(0.0f, -1.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}
				else
				{
					// 走っている場合は頭の先からそこそこ低い位置の間で当たっているかを判定( 傾斜で落下状態に移行してしまわない為 )
					lineRes = HitCheck_Line_Triangle(VAdd(moveAfterPos, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), VAdd(moveAfterPos, VGet(0.0f, -40.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}

				// 当たっていなかったら何もしない
				if (!lineRes.HitFlag) continue;

				// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
				if (hitFlag && MaxY > lineRes.Position.y) continue;

				// ポリゴンに当たったフラグを立てる
				hitFlag = true;

				// 接触したＹ座標を保存する
				MaxY = lineRes.Position.y;
			}

			// 床ポリゴンに当たったかどうかで処理を分岐
			if (hitFlag)
			{
				//// 当たった場合 ////

				// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
				moveAfterPos.y = MaxY;
			}
			else
			{
				//// 当たっていない場合 ////

				moveAfterPos.y -= 20;
			}
		}
		else
		{
			// 1枚もフィールドポリゴンと当たっていない場合落下
			moveAfterPos.y -= 20;
		}

		// 検出したプレイヤーの周囲のポリゴン情報を開放する
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
		// フェード処理
		FadeInUpdate(input);
	}

	// 各クラスの更新処理
	pSkyDoom_->Update();
	pFieldManager_->Update();
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
	if (pPlayer_->GetIsDead())
	{
		gameOverFadeTimer_++;
		gameOverUIfadeValue_ = static_cast<int>(255 * (static_cast<float>(gameOverFadeTimer_)) / static_cast<float>(game_over_fade_interval));
		if (gameOverFadeTimer_ >= 100)
		{
			gameOverFadeTimer_ = 100;
		}	 
	}

	// シーン切り替え
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