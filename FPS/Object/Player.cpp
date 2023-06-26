#include "Player.h"
#include "../Model.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Scene/MainScene.h"
#include "../Object/Field.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	// モデルのファイル名
	const char* const file_name = "data/model/player.mv1";

	// プレイヤーの移動量
	constexpr VECTOR player_vec_up{ 0, 0, -1 };
	constexpr VECTOR player_vec_down{ 0, 0, 1 };
	constexpr VECTOR player_vec_right{ -1, 0, 0 };
	constexpr VECTOR player_vec_left{ 1, 0, 0 };

	// プレイヤーの速度
	constexpr float player_speed = 20.0f;

	// ショットの発射位置
	constexpr VECTOR shot_firing_init_pos{ -89.264f, 150.0f, -260.0f };

	// ショットの速度
	constexpr float shot_speed = 100.0f;

	// ジャンプ力
	constexpr float jump_power = 16.0f;

	// 重力
	constexpr float gravity = -1.0f;

	// アニメーション番号
	constexpr int idle_anim_no = 3;			// 待機モーション
	constexpr int idle_shot_anim_no = 5;	// 停止している状態でショットを撃つ
	constexpr int walk_anim_no = 14;		// 移動モーション	
	constexpr int punch_anim_no = 10;		// 銃で殴るモーション
	constexpr int walk_shot_anim_no = 13;	// 移動している状態でショットを撃つ
	constexpr int dead_anim_no = 0;			// 死亡アニメーション
	constexpr int damage_anim_no = 2;		// ダメージアニメーション
	constexpr int fall_anim_no = 7;			// 落下アニメーション

	// アニメーション切り替わりにかかるフレーム数
	constexpr int anim_change_frame = 16;

	// フレーム番号
	constexpr int body_frame_no = 41;	// 体のフレーム
	constexpr int ears_frame_no = 42;	// 耳のフレーム
	constexpr int head_frame_no = 43;	// 頭のフレーム

	// 当たり半径のサイズ
	constexpr float col_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 100;

	// ダメージ食らった時の無敵時間
	constexpr int invincible_time = 60;

	// ショットの再使用まで待機フレーム数
	constexpr int shot_wait_time = 5;
}

Player::Player() :
	updateFunc_(&Player::UpdateIdle),
	animNo_(idle_anim_no),
	frameCount_(0),
	pos_(VGet(0, 0, 0)),
	jumpAcc_(0.0f),
	hp_(max_hp),
	damageFrame_(0),
	isMoving_(false),
	isDead_(false),
	moveVec_(VGet(0, 0, 0)),
	shotFrameCount_(0),
	isJump_(false),
	pMainScene_(nullptr)
{
}

Player::~Player()
{
}

void Player::Init()
{
	// 3Dモデルの生成
	pModel_ = std::make_shared<Model>(file_name);
	pModel_->SetAnimation(animNo_, true, true);
}

void Player::Update(const InputState& input)
{
	(this->*updateFunc_)(input);
}

void Player::Draw()
{
	// HPの描画
	for (int i = 0; i < hp_; i++)
	{
		DrawCircle(Game::screen_width - 400 + (i * 70), 100, 30, 0xff0000, true);
	}

	// ダメージ処理
	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// モデルの描画
	pModel_->Draw();
}

void Player::CollisionField()
{
	for (auto& field : pField_)
	{
		MV1_COLL_RESULT_POLY* poly;
		MV1_COLL_RESULT_POLY_DIM hitDim;
		HITRESULT_LINE lineRes;
		MV1_REF_POLYGONLIST refPoly = MV1GetReferenceMesh(pModel_->GetModelHandle(), -1, true);

		// 初期化
		bool hitFlag = false;
		int yukaNum = 0;
		int kabeNum = 0;

		// 移動前のプレイヤーの座標
		VECTOR oldPos = pos_;

		// 移動後のプレイヤーの座標
		VECTOR moveAfterPos = VAdd(oldPos, moveVec_);

		// プレイヤーとフィールドの当たり判定(何枚のポリゴンと当たっているか)
		hitDim = MV1CollCheck_Sphere(field->GetModelHandle(), -1, oldPos, (refPoly.MaxPosition.y + 50.0f) + VSize(moveVec_));

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
			if (isMoving_)
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
						SlideVec = VCross(moveVec_, poly->Normal);

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
				if (isJump_)
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

		pos_ = moveAfterPos;

		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);
	}
}

float Player::GetColRadius() const
{
	return col_radius;
}

void Player::OnDamage(int damage)
{
	// ダメージ処理
	if (damageFrame_ > 0)	return;
	hp_ -= damage;
	damageFrame_ = invincible_time;

	if (hp_ > 0)
	{
		// アニメーションをダメージアニメーションに変更
		animNo_ = damage_anim_no;
		pModel_->ChangeAnimation(damage_anim_no, false, false, 4);
		updateFunc_ = &Player::UpdateOnDamage;
		frameCount_ = 0;
	}
	else
	{
		// 死亡フラグを立てる
		isDead_ = true;

		// アニメーションを死亡アニメーションに変更
		animNo_ = dead_anim_no;
		pModel_->ChangeAnimation(dead_anim_no, false, false, 4);
		updateFunc_ = &Player::UpdateDead;
		frameCount_ = 0;
	}
}

void Player::SetVisible(bool visible)
{
	// trueの場合表示、falseの場合非表示
	if (visible)
	{
		// 表示
		MV1SetFrameVisible(pModel_->GetModelHandle(), ears_frame_no, true); // 耳
		MV1SetFrameVisible(pModel_->GetModelHandle(), head_frame_no, true);	// 頭
	}
	else
	{
		// 非表示
		MV1SetFrameVisible(pModel_->GetModelHandle(), ears_frame_no, false); // 耳
		MV1SetFrameVisible(pModel_->GetModelHandle(), head_frame_no, false); // 頭	
	}
}

void Player::UpdateIdle(const InputState& input)
{
	// フレームカウント
	shotFrameCount_++;

	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	//// ジャンプ処理
	//jumpAcc_ += gravity;
	//pos_.y += jumpAcc_;
	//if (pos_.y < 0.0f)
	//{
	////	pos_.y = 0.0f;
	//	jumpAcc_ = 0.0f;

	//	isJump_ = false;
	//}

	// ジャンプ処理
	if (!isJump_)
	{
		if (input.IsTriggered(InputType::jump))
		{
			jumpAcc_ = jump_power;
			isJump_ = true;
		}
	}

	// ショットを撃つ処理(ボタンが押されたとき、)
	if (input.IsPressed(InputType::shot) && shotFrameCount_ >= shot_wait_time)
	{
		// 弾の発射位置の作成
		MATRIX playerTransMtx = MGetTranslate(pos_);						// プレイヤーの平行移動行列の作成
		MATRIX cameraRotMtxSide = MGetRotY(pCamera_->GetCameraAngleX());	// 横移動情報の行列作成		
		MATRIX matrix = MMult(cameraRotMtxSide, playerTransMtx);			// 横移動情報行列とプレイヤーの平行移動行列の合成
		VECTOR shootStartPos = VTransform(shot_firing_init_pos, matrix);	// ショットの発射初期位置と作成した行列からベクトルの生成

		// レティクルの位置の取得
		VECTOR shotVec = ConvScreenPosToWorldPos(VGet(pMainScene_->GetReticlePosX(), pMainScene_->GetReticlePosY(), 1.0f));

		// 終点から始点を引く
		shotVec = VSub(shotVec, shootStartPos);

		// 正規化
		shotVec = VNorm(shotVec);

		// スピードかける
		shotVec = VScale(shotVec, shot_speed);

		// ショット開始
		pMainScene_->StartShot(shootStartPos, shotVec);

		// ショットアニメに変更する
		animNo_ = idle_shot_anim_no;
		pModel_->ChangeAnimation(animNo_, false, true, 4);

		// 初期化
		shotFrameCount_ = 0;
	}

	// プレイヤーの回転値を取得する
	VECTOR vect = MV1GetRotationXYZ(pModel_->GetModelHandle());

	// カメラが向いている方向からベクトル変換して移動情報作成
	VECTOR moveUp = VTransform(player_vec_up, MGetRotY(pCamera_->GetCameraAngleX()));
	VECTOR moveDown = VTransform(player_vec_down, MGetRotY(pCamera_->GetCameraAngleX()));
	VECTOR moveRight = VTransform(player_vec_right, MGetRotY(pCamera_->GetCameraAngleX() + vect.x));
	VECTOR moveLeft = VTransform(player_vec_left, MGetRotY(pCamera_->GetCameraAngleX() + vect.x));

	// 移動
	isMoving_ = false;
	moveVec_ = VGet(0, 0, 0);
	VECTOR moveVecX = VGet(0, 0, 0);
	VECTOR moveVecZ = VGet(0, 0, 0);
	if (input.IsPressed(InputType::w))
	{
		moveVecZ = moveUp;

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::a))
	{
		moveVecX = moveLeft;

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::s))
	{
		moveVecZ = moveDown;

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::d))
	{
		moveVecX = moveRight;

		isMoving_ = true;
	}

	if (isMoving_)
	{
		// x方向とz方向のベクトルを足して移動ベクトルを作成する
		moveVec_ = VAdd(moveVecZ, moveVecX);

		// 正規化
		moveVec_ = VNorm(moveVec_);

		// 正規化したベクトルにプレイヤーの速度をかける
		moveVec_ = VScale(moveVec_, player_speed);

		if (animNo_ == idle_anim_no)
		{
			// 歩行アニメに変更
			animNo_ = walk_anim_no;
			pModel_->ChangeAnimation(walk_anim_no, true, false, 4);
		}
	}
	else
	{
		if (animNo_ == walk_anim_no)
		{
			// 待機アニメに変更
			animNo_ = idle_anim_no;
			pModel_->ChangeAnimation(idle_anim_no, true, false, 4);
		}
	}

	// 当たり判定チェック
	pos_ = pMainScene_->ColisionToField(pModel_->GetModelHandle(), isMoving_, isJump_, pos_, moveVec_);

	// ショットアニメが終わり次第待機アニメに変更
	if (pModel_->IsAnimEnd() && animNo_ == idle_shot_anim_no)
	{
		animNo_ = idle_anim_no;
		pModel_->ChangeAnimation(idle_anim_no, true, true, 4);
	}

	// アニメーションを進める
	pModel_->Update();

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraAngleX(), 0.0f));
}

void Player::UpdateIdleShot(const InputState& input)
{
	// ショットアニメ以外でこのUpdateは呼ばれない
	assert(animNo_ == idle_shot_anim_no);

	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// アニメーション更新処理
	pModel_->Update();

	//// ジャンプ処理
	//jumpAcc_ += gravity;
	//pos_.y += jumpAcc_;
	//if (pos_.y < 0.0f)
	//{
	////	pos_.y = 0.0f;
	//	jumpAcc_ = 0.0f;
	//}

	frameCount_++;
	if (pModel_->IsAnimEnd())
	{
		// 待機アニメに変更する
		animNo_ = idle_anim_no;
		pModel_->ChangeAnimation(idle_anim_no, true, true, 4);

		// Updateを待機に
		updateFunc_ = &Player::UpdateIdle;
		frameCount_ = 0;
	}
}

void Player::UpdateDead(const InputState& input)
{
	assert(animNo_ == dead_anim_no);
	pModel_->Update();

	//// ジャンプ処理
	//jumpAcc_ += gravity;
	//pos_.y += jumpAcc_;
	//if (pos_.y < 0.0f)
	//{
	////	pos_.y = 0.0f;
	//	jumpAcc_ = 0.0f;
	//}
}

void Player::UpdateOnDamage(const InputState& input)
{
	assert(animNo_ == damage_anim_no);
	pModel_->Update();

	if (pModel_->IsAnimEnd())
	{
		// 待機アニメに変更する
		animNo_ = idle_anim_no;
		pModel_->ChangeAnimation(idle_anim_no, true, true, 4);

		// Updateを待機に
		updateFunc_ = &Player::UpdateIdle;
		frameCount_ = 0;
	}
}
