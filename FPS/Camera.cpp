#include "Camera.h"
#include "Object/Player.h"
#include "InputState.h"
#include "Game.h"
#include "Object/Tower.h"
#include <DxLib.h>

namespace
{
	// カメラの初期位置とカメラの初期注視点
//	constexpr VECTOR tps_camera_pos{ 0, 500, 300 };
	constexpr VECTOR tps_camera_pos{ 500, 300, -300 };
	constexpr VECTOR tps_camera_target{ 0, 0, 0 };
//	constexpr VECTOR tps_camera_target{ 0, 400, -120 };

	constexpr VECTOR fps_camera_pos{ 0, 200, -10 };
	constexpr VECTOR fps_camera_target{ 0, 90, 500 };

	// カメラ視野角の設定
	constexpr float perspective = 90.0f;

	// 描画距離(near, far)
	constexpr float near_distance = 5.0f;
	constexpr float far_distance = 18400.0f;

	// 倒れるアニメーションに何フレーム使うのか
	constexpr int anim_frame_num = 180;

	// 旋回速度
	constexpr float rot_speed = 0.01f;
}

Camera::Camera() :
	cameraPos_(VGet(0, 0, 0)),
	cameraInitPos_(VGet(0, 0, 0)),
	cameraTarget_(VGet(0, 0, 0)),
	cameraInitTarget_(VGet(0, 0, 0)),
	perspectiveFps_(true),
	mousePosX_(0),
	mousePosY_(0),
	mouseMoveX_(0.0f),
	mouseMoveY_(0.0f),
	cameraYaw_(0.0f),
	cameraPitch_(0.0f),
	isMouseScreenEdge_(false),
	deadFrame_(0),
	distance_(VGet(0, 0, 0)),
	upVector_(VGet(0, 0, 0)),
	size_(0.0f),
	quakeTimer_(0),
	quake_(VGet(0, 0, 0))
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
#ifdef _DEBUG
	SetMouseDispFlag(true);
#else
	// マウスカーソルの非表示
	SetMouseDispFlag(false);
#endif 

	// マウスポインタの初期位置の設定
	SetMousePoint(Game::screen_width / 2, Game::screen_height / 2);

	// プレイヤーの特定のモデルの非表示
	pPlayer_->SetVisible(false);

	// 指定した位置にカメラを設置
	cameraPos_ = fps_camera_pos;
	cameraInitPos_ = fps_camera_pos;

	// カメラの注視点の設定
	cameraInitTarget_ = fps_camera_target;

	// カメラからどれだけ離れたところ( Near )から、 どこまで( Far )のものを描画するかを設定
	SetCameraNearFar(near_distance, far_distance);;

	// カメラの位置、どこを見ているかを設定する
	SetCameraPositionAndTargetAndUpVec(cameraPos_, cameraInitTarget_, VGet(0, 1, 0));

	// カメラの視野角を設定(ラジアン)
	SetupCamera_Perspective(perspective * DX_PI_F / 180.0f);
}

void Camera::Update(const InputState& input)
{
	// プレイヤーが生きている場合のみマウスの座標の取得
	if (!pPlayer_->GetIsDead() && !pTower_->GetIsDead())
	{
		GetMousePoint(&mousePosX_, &mousePosY_);
	}

	// マウスカーソルの位置の初期化
	SetMousePoint(Game::screen_width / 2, Game::screen_height / 2);

	// マウスをどのくらい動かしたのか
	mouseMoveX_ = static_cast<float>(mousePosX_ - (Game::screen_width / 2));
	mouseMoveY_ = static_cast<float>(mousePosY_ - (Game::screen_height / 2));

	// カメラの回転にマウスの移動量を足す
	cameraYaw_ += mouseMoveX_ * rot_speed;
	cameraPitch_ += mouseMoveY_ * rot_speed;

	// カメラの回転の制限
	if (cameraPitch_ <= 150 * rot_speed)	cameraPitch_ = 150 * rot_speed;
	if (cameraPitch_ >= 350 * rot_speed)	cameraPitch_ = 350 * rot_speed;

	// Y軸のカメラの追従
	VECTOR cameraTrans = pPlayer_->GetPos();
	//	cameraTrans.y = 0.0f;							// Y軸カメラの追従を行わない
	//	cameraTrans.y = pPlayer_->GetPos().y * 0.65f;	// Y軸カメラの追従を少し遅くする

	// 平行行列の作成(なにこれ??)
	MATRIX playerTransMtx = MGetTranslate(cameraTrans);

	// プレイヤーの回転に合わせてカメラの位置、注視点を回転させる
	// プレイヤーの回転情報と平行移動情報を合成
	MATRIX cameraRotMtxSide = MGetRotY(cameraYaw_);				// 横移動情報の作成
	MATRIX cameraRotMtxVertical = MGetRotX(-cameraPitch_);		// 縦移動情報の作成
	MATRIX cameraRotMtx = MMult(cameraRotMtxVertical, cameraRotMtxSide);
	MATRIX cameraMtxPos = MMult(cameraRotMtxSide, playerTransMtx);
	MATRIX cameraMtxTarget = MMult(cameraRotMtx, playerTransMtx);
	cameraPos_ = VTransform(cameraInitPos_, cameraMtxPos);

	// 生きている場合のみプレイヤーにカメラを追従
	if (!pPlayer_->GetIsDead())
	{
		cameraTarget_ = VTransform(cameraInitTarget_, cameraMtxTarget);
	//	cameraTarget_ = cameraInitTarget_;
	}
	// プレイヤーが死んでいたらカメラを死亡時カメラに切り替え
	else if(pPlayer_->GetIsDead() && perspectiveFps_)
	{
		// 一回しか通らない
		if (!isPass_)
		{
			// カメラの見ている座標からカメラの位置を引いてベクトルを求める
			distance_ = VSub(cameraTarget_, cameraPos_);

			// ベクトルから距離(大きさ)を求める
			size_ = VSize(distance_);

			// 距離を見たい方向に割り振る
			upVector_ = VGet(0, size_, 0);

			// 通ったフラグを立てる
			isPass_ = true;
		}

		// フレーム加算
		if (deadFrame_++ >= anim_frame_num - 1)
		{
			deadFrame_ = anim_frame_num - 1;
		}

		// 割合計算
		float ratio = static_cast<float>(deadFrame_) / static_cast<float>(anim_frame_num); // 0～100
		float ratio2 = 1.0f - ratio;													   // 100～0	

		// カメラの見ている方向のベクトルとカメラの位置からカメラの見ている座標を求める
		cameraTarget_ = VAdd(VAdd(VScale(distance_, ratio2), VScale(upVector_, ratio)), cameraPos_);
	}

	// カメラからどれだけ離れたところ( Near )から、 どこまで( Far )のものを描画するかを設定
	SetCameraNearFar(near_distance, far_distance);

	// カメラの視野角を設定(ラジアン)
	SetupCamera_Perspective(perspective * DX_PI_F / 180.0f);

	// カメラの位置、どこを見ているかを設定する
	SetCameraPositionAndTargetAndUpVec(cameraPos_, cameraTarget_, VGet(0, 1, 0));
	
	// カメラ切り替え
#ifdef _DEBUG
	if (input.IsTriggered(InputType::perspective))
	{
		if (perspectiveFps_)
			perspectiveFps_ = false;
		else
			perspectiveFps_ = true;
	}
	if (perspectiveFps_)
	{
		// カメラの位置の切り替え
		cameraPos_ = fps_camera_pos;
		cameraInitPos_ = fps_camera_pos;

		// プレイヤーが生きている場合特定のモデルの非表示
		if (!pPlayer_->GetIsDead())
		{
			pPlayer_->SetVisible(false);
		}
	}
	else
	{
		// カメラの位置の切り替え
		cameraPos_ = tps_camera_pos;
		cameraInitPos_ = tps_camera_pos;

		// プレイヤーの特定のモデルの表示
		pPlayer_->SetVisible(true);
	}
#endif

	// カメラを揺らす
	Quake();
}

void Camera::Draw()
{
	DrawFormatString(10, 180, 0x000000, "mousePos = %d, %d", mousePosX_, mousePosY_);
}

float Camera::GetCameraYaw() const
{
	return cameraYaw_;
}

float Camera::GetCameraPitch() const
{
	return cameraPitch_;
}

void Camera::Quake()
{
	if (quakeTimer_ > 0)
	{
		quake_ = VScale(quake_, -0.95f);

		--quakeTimer_;
	}
	else
	{
		quake_ = VGet(0, 0, 0);
	}

	cameraInitPos_ = VAdd(cameraInitPos_, quake_);
}
