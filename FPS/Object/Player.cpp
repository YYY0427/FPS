#include "Player.h"
#include "../Model.h"
#include "../Game.h"
#include "../InputState.h"
#include "../Camera.h"
#include "../Scene/MainScene.h"
#include "../Collision.h"
#include "../BomManager.h"
#include "Tower.h"
#include "../SoundManager.h"
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
	constexpr float player_speed = 15.0f;

	// ショットの発射位置
	constexpr VECTOR shot_firing_init_pos{ -89.264f, 150.0f, -260.0f };

	// ショットの速度
	constexpr float shot_speed = 150.0f;

	// ボムショットの速度
	constexpr float bom_speed = 50.0f;

	// ジャンプ力
	constexpr float jump_power = 45.0f;

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
	constexpr float collision_radius = 70.0f;

	// 最大HP
	constexpr int max_hp = 200;

	// ダメージ食らった時の無敵時間
	constexpr int invincible_time = 0;

	// ショットの再使用まで待機フレーム数
	constexpr int shot_wait_time = 5;

	// 爆弾の再使用まで待機フレーム数
	constexpr int bom_wait_time = 60 * 20;

	// リスポーン地点
	constexpr VECTOR respawn_point{ 6000.0f, 0.0f, 2200.0f };

	// 初期位置
//	constexpr VECTOR init_pos_1{ 6000.0f, 0.0f, 2200.0f };
	constexpr VECTOR init_pos_1{ 6616, -223, 3251 };
}

Player::Player(MainScene* pMainScene) :
	pMainScene_(pMainScene),
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
	pCollision_(nullptr),
	isFall_(false),
	bomFrameCount_(0),
	isUseBom_(true),
	isUseShot_(true)
{
	// 3Dモデルの生成
	pModel_ = std::make_shared<Model>(file_name);

	// アニメーション設定
	pModel_->SetAnimation(animNo_, true, true);
	pModel_->SetUseCollision(true);

	// 初期位置
	pos_ = init_pos_1;
	pModel_->SetPos(pos_);
	pModel_->Update();
}

Player::~Player()
{
	
}

void Player::Update(const InputState& input)
{
	(this->*updateFunc_)(input);
}

void Player::Draw()
{
	// ダメージ処理
	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// モデルの描画
	pModel_->Draw();

#ifdef _DEBUG
	DrawFormatString(20, 300, 0x000000, "playerPos = %f, %f, %f", pos_.x, pos_.y, pos_.z);
	DrawFormatString(20, 250, 0x000000, "playerGroundY = %f", pCollision_->GetGroundY());
	DrawSphere3D(pos_, collision_radius, 16.0f, 0xff0000, 0xff0000, false);
#endif
}

void Player::SetRespawn()
{
	isFall_ = false;
	OnDamage(1);
	pos_ = respawn_point;
}

int Player::GetMaxHP() const
{
	return max_hp;
}

float Player::GetCollisionRadius() const
{
	return collision_radius;
}

void Player::OnDamage(int damage)
{
	if (damageFrame_ > 0)	return;
	if (pTower_->GetIsDead()) return;

	// ダメージ処理
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
		hp_ = 0;

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
	auto& soundManager = SoundManager::GetInstance();

	// フレームカウント
	shotFrameCount_--;
	if (shotFrameCount_ < 0) shotFrameCount_ = 0;
	if (shotFrameCount_ <= 0) isUseShot_ = true;

	bomFrameCount_--;
	if (bomFrameCount_ < 0) bomFrameCount_ = 0;
	if (bomFrameCount_ <= 0) isUseBom_ = true;

	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// タワーが死んでいたらプレイヤーを動かさない
	if (pTower_->GetIsDead())
	{
		animNo_ = dead_anim_no;
		updateFunc_ = &Player::UpdateDead;
	}

	// ジャンプ処理
	if (!isJump_)
	{
		if (input.IsPressed(InputType::jump))
		{
			jumpAcc_ = jump_power;
			isJump_ = true;
		}
	}

	// ショットを撃つ処理
	if (input.IsPressed(InputType::shot) && isUseShot_ && !pMainScene_->GetIsGameStop())
	{
		soundManager.Play("gun");

		// 弾の発射位置の作成
		MATRIX playerTransMtx = MGetTranslate(pos_);						// プレイヤーの平行移動行列の作成
		MATRIX cameraRotMtxSide = MGetRotY(pCamera_->GetCameraYaw());		// 横移動情報の行列作成		
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
		pMainScene_->StartPlayerShot(shootStartPos, shotVec);

		// ショットアニメに変更する
		animNo_ = idle_shot_anim_no;
		pModel_->ChangeAnimation(animNo_, false, true, 4);

		// 初期化
		shotFrameCount_ = shot_wait_time;
		isUseShot_ = false;
	}

	if (input.IsTriggered(InputType::bom) && isUseBom_ && !pMainScene_->GetIsGameStop())
	{
		soundManager.Play("bom");

		// 弾の発射位置の作成
		MATRIX playerTransMtx = MGetTranslate(pos_);						// プレイヤーの平行移動行列の作成
		MATRIX cameraRotMtxSide = MGetRotY(pCamera_->GetCameraYaw());		// 横移動情報の行列作成		
		MATRIX matrix = MMult(cameraRotMtxSide, playerTransMtx);			// 横移動情報行列とプレイヤーの平行移動行列の合成
		VECTOR shootStartPos = VTransform(shot_firing_init_pos, matrix);	// ショットの発射初期位置と作成した行列からベクトルの生成

		// レティクルの位置の取得
		VECTOR shotVec = ConvScreenPosToWorldPos(VGet(pMainScene_->GetReticlePosX(), pMainScene_->GetReticlePosY(), 1.0f));

		// 終点から始点を引く
		shotVec = VSub(shotVec, shootStartPos);

		// 正規化
		shotVec = VNorm(shotVec);

		// スピードかける
		shotVec = VScale(shotVec, bom_speed);

		pBomManager_->StartBom(shootStartPos, shotVec, pCamera_->GetCameraYaw());

		// ショットアニメに変更する
		animNo_ = idle_shot_anim_no;
		pModel_->ChangeAnimation(animNo_, false, true, 4);

		bomFrameCount_ = bom_wait_time;
		isUseBom_ = false;
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


	if (isMoving_ && !pMainScene_->GetIsGameStop())
	{
		// x方向とz方向のベクトルを足して移動ベクトルを作成する
		moveVec_ = VAdd(moveVecZ, moveVecX);

		// 正規化
		moveVec_ = VNorm(moveVec_);

		// 正規化したベクトルにプレイヤーの速度をかける
		moveVec_ = VScale(moveVec_, player_speed);

		if (!soundManager.CheckMusic("run"))
		{
			soundManager.Play("run");
		}

		if (animNo_ == idle_anim_no)
		{
			// 歩行アニメに変更
			animNo_ = walk_anim_no;
			pModel_->ChangeAnimation(walk_anim_no, true, false, 4);
		}
	}
	else
	{
		soundManager.StopSelectMusic("run");

		if (animNo_ == walk_anim_no)
		{
			// 待機アニメに変更
			animNo_ = idle_anim_no;
			pModel_->ChangeAnimation(idle_anim_no, true, false, 4);
		}
	}

	// 当たり判定チェック
	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), isMoving_, isJump_, true, pos_, moveVec_, Collision::Chara::player, collision_radius);

	// ジャンプ処理
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y < pCollision_->GetGroundY())
	{
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}

	if (pos_.y  > pCollision_->GetGroundY() + 50.0f)
	{
		soundManager.StopSelectMusic("run");
	}

	// 地面から落ちた場合ダメージをくらい、リスポーン地点に戻る
	if (pos_.y <  - 3000.0f)
	{
		isFall_ = true;
		pMainScene_->PlayerFallFade();
	}

	// ショットアニメが終わり次第待機アニメに変更
	if (pModel_->IsAnimEnd() && animNo_ == idle_shot_anim_no)
	{
		animNo_ = idle_anim_no;
		pModel_->ChangeAnimation(idle_anim_no, true, true, 4);
	}

	soundManager.Set3DSoundListenerPosAndFrontPos_UpVecY(pos_, VGet(-cos(pCamera_->GetCameraYaw()), 0, sin(pCamera_->GetCameraYaw())));

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraYaw(), 0.0f));

	// アニメーションを進める
	pModel_->Update();
}

void Player::UpdateIdleShot(const InputState& input)
{
	// ショットアニメ以外でこのUpdateは呼ばれない
	assert(animNo_ == idle_shot_anim_no);

	// ダメージ処理
	damageFrame_--;
	if (damageFrame_ < 0) damageFrame_ = 0;

	// ジャンプ処理
	jumpAcc_ += gravity;
	pos_.y += jumpAcc_;
	if (pos_.y <= pCollision_->GetGroundY())
	{
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}

	// 位置座標の設定
	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraYaw(), 0.0f));

	// アニメーションを進める
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
	if (pos_.y <= pCollision_->GetGroundY())
	{
		jumpAcc_ = 0.0f;
		isJump_ = false;
	}
	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), isMoving_, false, true, pos_, VGet(0, 0, 0), Collision::Chara::player, collision_radius);
}

void Player::UpdateOnDamage(const InputState& input)
{
	assert(animNo_ == damage_anim_no);

	pos_ = pCollision_->ExtrusionColision(pModel_->GetModelHandle(), isMoving_, isJump_, true, pos_, moveVec_, Collision::Chara::player, collision_radius);

	pModel_->SetPos(pos_);

	// 向いている方向の設定
	pModel_->SetRot(VGet(0.0f, pCamera_->GetCameraYaw(), 0.0f));

	// アニメーションを進める
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
