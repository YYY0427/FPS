#include "Player.h"
#include "../Model.h"
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
	constexpr float shot_speed = 100.0f;

	// ジャンプ力
	constexpr float jump_power = 16.0f;

	// 重力
	constexpr float gravity = -1.0f;

	// アニメーション番号
	constexpr int idle_anim_no = 3;			// 待機モーション
	constexpr int idle_shot_anim_no = 5;	// 停止している状態でショットを撃つ
	constexpr int walk_anim_no = 14;		// 移動モーション	

	// アニメーション切り替わりにかかるフレーム数
	constexpr int anim_change_frame = 16;

	// 当たり半径のサイズ
	constexpr float col_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 5;
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
	damageFrame_ = 60 * 2;
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
		MV1SetFrameVisible(pModel_->GetModelHandle(), 41, false);
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
		// ショットを撃つ処理
		// Bボタンでショット
		if (input.IsTriggered(InputType::shot))
		{
			// ショットを発射する
			VECTOR shootStart = pos_;	// 弾の発射位置
			shootStart.y = 80.0f;

			// ショットを発射する
			VECTOR shotVec = moveZ;

			shotVec = VNorm(shotVec);
			shotVec = VScale(shotVec, shot_speed);

			//for (int i = 0; i < 21; i++)
			//{
			//	// 発射方向を生成 -90度～90度
			//	float angle = (-90 + 9 * i) * DX_PI_F / 180.0f;
			//	MATRIX rotMtx = MGetRotY(angle);
			//	VECTOR rotVec = VTransform(shotVec, rotMtx);
			//	pMainScene_->StartShot(shootStart, rotVec);
			//}

			pMainScene_->StartShot(shootStart, shotVec);

			// ショットアニメに変更する
			animNo_ = idle_shot_anim_no;
			pModel_->ChangeAnimation(animNo_, false, true, 4);

			// Updateをショットに
			updateFunc_ = &Player::UpdateIdleShot;
			frameCount_ = 0;
		}
	}

	// 上下キーで前後移動
	bool isMoving = false;
	if (input.IsPressed(InputType::w))
	{
		pos_ = VAdd(pos_, moveZ);

		isMoving = true;
	}
	if (input.IsPressed(InputType::a))
	{
		pos_ = VSub(pos_, moveX);

		isMoving = true;
	}
	if (input.IsPressed(InputType::s))
	{
		pos_ = VSub(pos_, moveZ);

		isMoving = true;
	}
	if (input.IsPressed(InputType::d))
	{
		pos_ = VAdd(pos_, moveX);

		isMoving = true;
	}

	if (isMoving)
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
	}

	pModel_->SetPos(pos_);
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraAngle(), 0.0f));
}

void Player::UpdateIdleShot(const InputState& input)
{
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

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