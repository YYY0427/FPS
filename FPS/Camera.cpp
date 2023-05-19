#include "Camera.h"
#include "Object/Player.h"
#include "InputState.h"
#include "Game.h"
#include <DxLib.h>

namespace
{
	// カメラの初期位置
	constexpr VECTOR tps_camera_pos{ 0, 720, 620 };
	constexpr VECTOR tps_camera_target{ 0, 400, -120 };
	constexpr VECTOR fps_camera_pos{ 0, 200, 21 };
	constexpr VECTOR fps_camera_target{ 0, 90, -500 };

	// カメラ視野角の設定
	constexpr float perspective = 90.0f;

	// 旋回速度
	constexpr float rot_speed = 0.01f;
}

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
	// マウスカーソルの非表示
	SetMouseDispFlag(false);

	// マウスポインタの初期位置の設定
	SetMousePoint(Game::screen_width / 2, Game::screen_height / 2);

	// プレイヤーの特定のモデルの非表示
	pPlayer_->SetVisible(false);

	// 指定した位置にカメラを設置
	cameraPos_ = fps_camera_pos;
	cameraInitPos_ = fps_camera_pos;

	// カメラの向いている方向の設定
	cameraAngle_ = angle_;

	// カメラからどれだけ離れたところ( Near )から、 どこまで( Far )のものを描画するかを設定
	SetCameraNearFar(5.0f, 2800.0f);

	// カメラの位置、どこを見ているかを設定する
	SetCameraPositionAndTargetAndUpVec(cameraPos_, fps_camera_target, VGet(0, 1, 0));

	// カメラの視野角を設定(ラジアン)
	SetupCamera_Perspective(perspective * DX_PI_F / 180.0f);

	reticlePos_ = ConvScreenPosToWorldPos(VGet(Game::screen_width / 2, Game::screen_height, 1.0f));
}

void Camera::Update(const InputState& input)
{
#if false
	cameraAngle_ = pPlayer_->GetAngle();
#else
	cameraAngle_ = (cameraAngle_ * 0.8f) + (angle_ * 0.2f);
#endif

	// マウスの座標の取得
	GetMousePoint(&mousePosX_, &mousePosY_);

	// マウスをどのくらい動かしたのか
	mouseMoveX_ = mousePosX_ - (Game::screen_width / 2);
	if (mousePosY_ <= Game::screen_height / 2 + 100 &&	// マウスの動かす範囲の制限
		mousePosY_ >= Game::screen_height / 2 - 100)		
	{
		mouseMoveY_ = mousePosY_ - (Game::screen_height / 2);
	}

	// カメラ切り替え
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

		// プレイヤーの特定のモデルの非表示
		pPlayer_->SetVisible(false);
	}
	else
	{
		// カメラの位置の切り替え
		cameraPos_ = tps_camera_pos;
		cameraInitPos_ = tps_camera_pos;

		// プレイヤーの特定のモデルの表示
		pPlayer_->SetVisible(true);
	}

	// Y軸のカメラの追従
	// ジャンプ時は単純にプレイヤーに追従するのではなく
	// プレイヤーの立っていた位置を見るようにする
	VECTOR cameraTrans = pPlayer_->GetPos();
//	cameraTrans.y = 0.0f;							// Y軸カメラの追従を行わない
//	cameraTrans.y = pPlayer_->GetPos().y * 0.65f;	// Y軸カメラの追従を少し遅くする
	cameraTrans.y = pPlayer_->GetPos().y;			// Y軸カメラの追従をプレイヤーの位置に合わせる

	MATRIX playerTransMtx = MGetTranslate(cameraTrans);
	
	// プレイヤーの回転に合わせてカメラの位置、注視点を回転させる
	// プレイヤーの回転情報と平行移動情報を合成
	MATRIX cameraRotMtxSide = MGetRotY(mouseMoveX_ * rot_speed);	// 横移動情報の作成
	MATRIX cameraRotMtxVertex = MGetRotX(mouseMoveY_ * rot_speed);	// 縦移動情報の作成
	MATRIX cameraRotMtx = MMult(MTranspose(cameraRotMtxVertex), cameraRotMtxSide);
	MATRIX cameraMtxPos = MMult(cameraRotMtxSide, playerTransMtx);
	MATRIX cameraMtxTarget = MMult(cameraRotMtx, playerTransMtx);
	cameraPos_ = VTransform(cameraInitPos_, cameraMtxPos);
	cameraTarget_ = VTransform(fps_camera_target, cameraMtxTarget);

	// カメラからどれだけ離れたところ( Near )から、 どこまで( Far )のものを描画するかを設定
	SetCameraNearFar(5.0f, 2800.0f);

	// カメラの視野角を設定(ラジアン)
	SetupCamera_Perspective(perspective * DX_PI_F / 180.0f);

	// カメラの位置、どこを見ているかを設定する
	SetCameraPositionAndTargetAndUpVec(cameraPos_, cameraTarget_, VGet(0, 1, 0));
}

void Camera::Draw()
{
	// カメラの位置の表示
//	DrawSphere3D(cameraPos_, 30.0f, 1, GetColor(255, 0, 0), GetColor(255, 255, 255), true);
}

float Camera::GetCameraAngle() const
{
	return mouseMoveX_ * rot_speed;
}
