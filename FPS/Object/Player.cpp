#include "Player.h"
#include "../Model.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Scene/MainScene.h"
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

	// ショットの速度
	constexpr float shot_speed = 50.0f;

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
	constexpr int damage_anim_no = 2;		// 死亡アニメーション

	// アニメーション切り替わりにかかるフレーム数
	constexpr int anim_change_frame = 16;

	// フレーム番号
	constexpr int body_frame_no = 41;	// 体のフレーム
	constexpr int ears_frame_no = 42;	// 耳のフレーム
	constexpr int head_frame_no = 43;	// 頭のフレーム

	// 当たり半径のサイズ
	constexpr float col_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 1;

	// ダメージ食らった時の無敵時間
	constexpr int invincible_time = 60;
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
	moveVec_(VGet(0, 0, 0))
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
	// アニメーションを進める
	pModel_->Update();

	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// ジャンプ処理
	bool isJumping = true;	
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y < 0.0f)
	{
		pos_.y = 0.0f;
		jumpAcc_ = 0.0f;

		isJumping = false;
	}

	// ジャンプ処理
	if (!isJumping)
	{
		if (input.IsTriggered(InputType::jump))
		{
			jumpAcc_ = jump_power;
		}
	}

	// ショットを撃つ処理
	if (input.IsPressed(InputType::shot))
	{
		// 弾の発射位置
		VECTOR shootStart = MV1GetFramePosition(pModel_->GetModelHandle(), 27);
		shootStart.y = 100.0f;

		// レティクルの位置の取得
		VECTOR shotVec = ConvScreenPosToWorldPos(VGet(pMainScene_->GetReticlePosX(), pMainScene_->GetReticlePosY(), 1.0f));

		// 終点から始点を引く
		shotVec = VSub(shotVec, shootStart);

		// 正規化
		shotVec = VNorm(shotVec);

		// スピードかける
		shotVec = VScale(shotVec, shot_speed);

		// ショット開始
		pMainScene_->StartShot(shootStart, shotVec);

		// 止まっている場合と走っている場合で分岐
	//	if (!isMoving_)
		{
			// ショットアニメに変更する
			animNo_ = idle_shot_anim_no;
			pModel_->ChangeAnimation(animNo_, false, true, 4);

			updateFunc_ = &Player::UpdateIdleShot;
			frameCount_ = 0;
		}
		//else
		//{
		//	// ショット歩行アニメに変更
		//	animNo_ = walk_shot_anim_no;
		//	pModel_->ChangeAnimation(walk_shot_anim_no, true, false, 4);
		//}
	}

	// プレイヤーの回転値を取得する
	VECTOR vect = MV1GetRotationXYZ(pModel_->GetModelHandle());

	// カメラが向いている方向からベクトル変換して移動情報作成
	VECTOR moveUp = VTransform(player_vec_up, MGetRotY(pCamera_->GetCameraAngle()));
	VECTOR moveDown = VTransform(player_vec_down, MGetRotY(pCamera_->GetCameraAngle()));
	VECTOR moveRight = VTransform(player_vec_right, MGetRotY(pCamera_->GetCameraAngle() + vect.x));
	VECTOR moveLeft = VTransform(player_vec_left, MGetRotY(pCamera_->GetCameraAngle() + vect.x));

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

		// 動かす
		pos_ = VAdd(pos_, moveVec_);

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
		else if (animNo_ == walk_shot_anim_no)
		{
			// 待機アニメに変更
			animNo_ = idle_anim_no;
			pModel_->ChangeAnimation(idle_anim_no, true, false, 4);
		}
	}

	pModel_->SetPos(pos_);
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraAngle(), 0.0f));
}

void Player::UpdateIdleShot(const InputState& input)
{
	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// ジャンプ処理
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y < 0.0f)
	{
		pos_.y = 0.0f;
		jumpAcc_ = 0.0f;
	}

	// ショットアニメ以外でこのUpdateは呼ばれない
	assert(animNo_ == idle_shot_anim_no);
	pModel_->Update();

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
	if (pos_.y < 0.0f)
	{
		pos_.y = 0.0f;
		jumpAcc_ = 0.0f;
	}

	if (pModel_->IsAnimEnd())
	{

	}
}

void Player::UpdateOnDamage(const InputState& input)
{
	assert(animNo_ == damage_anim_no);
	pModel_->Update();

	if (pModel_->IsAnimEnd())
	{
	//	if (!isMoving_)
		{
			// 待機アニメに変更する
			animNo_ = idle_anim_no;
			pModel_->ChangeAnimation(idle_anim_no, true, true, 4);

			// Updateを待機に
			updateFunc_ = &Player::UpdateIdle;
			frameCount_ = 0;
		}
	//	else
		{

		}
	}
}
