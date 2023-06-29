#include "Player.h"
#include "../Model.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Scene/MainScene.h"
#include "../Collision.h"
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
	constexpr int max_hp = 10;

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
	pMainScene_(nullptr),
	pCollision_(nullptr)
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
		DrawCircle(100 + (i * 70), 130, 30, 0xff0000, true);
	}

	// ダメージ処理
	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// モデルの描画
	pModel_->Draw();
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

	// ジャンプ処理
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y <= pCollision_->GetMaxY())
	{
	//	pos_.y = pMainScene_->GetMaxY();
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}

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
		MATRIX cameraRotMtxSide = MGetRotY(pCamera_->GetCameraYaw());	// 横移動情報の行列作成		
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
	VECTOR moveUp = VTransform(player_vec_up, MGetRotY(pCamera_->GetCameraYaw()));
	VECTOR moveDown = VTransform(player_vec_down, MGetRotY(pCamera_->GetCameraYaw()));
	VECTOR moveRight = VTransform(player_vec_right, MGetRotY(pCamera_->GetCameraYaw() + vect.x));
	VECTOR moveLeft = VTransform(player_vec_left, MGetRotY(pCamera_->GetCameraYaw() + vect.x));

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
//	pos_ = pMainScene_->ColisionToField(pModel_->GetModelHandle(), isMoving_, isJump_, pos_, moveVec_);
	pos_ = pCollision_->ColisionToField(pModel_->GetModelHandle(), isMoving_, isJump_, pos_, moveVec_);

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
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraYaw(), 0.0f));
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

	// ジャンプ処理
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y <= pCollision_->GetMaxY())
	{
	//	pos_.y = pMainScene_->GetMaxY();
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}

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

	// ジャンプ処理
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y <= pCollision_->GetMaxY())
	{
		pos_.y = pCollision_->GetMaxY();
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}
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
