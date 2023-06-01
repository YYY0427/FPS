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
	constexpr VECTOR player_vec_z{ 0, 0, -10 };
	constexpr VECTOR player_vec_x{ -10, 0, 0 };
	constexpr VECTOR player_vec_y{ 0, -10, 0 };

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

	// アニメーション切り替わりにかかるフレーム数
	constexpr int anim_change_frame = 16;

	// 当たり半径のサイズ
	constexpr float col_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 5;

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
	damageFrame_(0)
{
	// 3Dモデルの生成
	pModel_ = std::make_shared<Model>(file_name);
	pModel_->SetAnimation(animNo_, true, true);
}

Player::~Player()
{
}

void Player::Init()
{
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

	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// モデルの描画
	pModel_->Draw();
}

float Player::GetColRadius()
{
	return col_radius;
}

void Player::OnDamage(int damage)
{
	if (damageFrame_ > 0)	return;

	hp_ -= damage;
	damageFrame_ = invincible_time;
}

void Player::SetVisible(bool visible)
{
	if (visible)
	{
		MV1SetFrameVisible(pModel_->GetModelHandle(), 43, true);
		MV1SetFrameVisible(pModel_->GetModelHandle(), 41, true);
		MV1SetFrameVisible(pModel_->GetModelHandle(), 42, true);
	}
	else
	{
		MV1SetFrameVisible(pModel_->GetModelHandle(), 43, false);
	//	MV1SetFrameVisible(pModel_->GetModelHandle(), 41, false);
		MV1SetFrameVisible(pModel_->GetModelHandle(), 42, false);
	}
}

void Player::UpdateIdle(const InputState& input)
{
	// アニメーションを進める
	pModel_->Update();

	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// ジャンプ処理
	bool isJumping = true;	// ジャンプしているフラグ
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y < 0.0f)
	{
		pos_.y = 0.0f;
		jumpAcc_ = 0.0f;

		isJumping = false;
	}

	// プレイヤーが向いている方向によって進む方向を決める
	VECTOR vect = MV1GetRotationXYZ(pModel_->GetModelHandle());

	VECTOR moveZ = VTransform(player_vec_z, MGetRotY(pCamera_->GetCameraAngle()));
	VECTOR moveX = VTransform(player_vec_x, MGetRotY(pCamera_->GetCameraAngle() + vect.x));

	if (!isJumping)
	{
		// Aボタンでジャンプ
		if (input.IsTriggered(InputType::jump))
		{
			jumpAcc_ = jump_power;
		}
	}

	// ショットを撃つ処理
	if (input.IsPressed(InputType::shot))
	{
		// 弾の発射位置
		VECTOR shootStart = pos_;	
		shootStart.y = 80.0f;

		// レティクルの位置の取得
		VECTOR shotVec = ConvScreenPosToWorldPos(VGet(pMainScene_->GetReticlePosX(), pMainScene_->GetReticlePosY(), 1.0f));

		// 終点から始点を引く
		shotVec = VSub(shotVec , pos_);
		
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

	// 移動
	isMoving_ = false;
	if (input.IsPressed(InputType::w))
	{
		pos_ = VAdd(pos_, moveZ);

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::a))
	{
		pos_ = VSub(pos_, moveX);

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::s))
	{
		pos_ = VSub(pos_, moveZ);

		isMoving_ = true;
	}
	if (input.IsPressed(InputType::d))
	{
		pos_ = VAdd(pos_, moveX);

		isMoving_ = true;
	}

	if (isMoving_)
	{
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
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

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
