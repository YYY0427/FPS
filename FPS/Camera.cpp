#include "Camera.h"
#include "Object/Player.h"
#include "InputState.h"
#include "Game.h"
#include <DxLib.h>

namespace
{
	// カメラの初期位置
	constexpr VECTOR tps_camera_pos{ 0, 500, 300 };
	constexpr VECTOR tps_camera_target{ 0, 400, -120 };

	constexpr VECTOR fps_camera_pos{ 0, 200, -10 };
	constexpr VECTOR fps_camera_target{ 0, 90, -500 };
	constexpr VECTOR fps_dead_camera_target{0, 500, 100};

	// カメラ視野角の設定
	constexpr float perspective = 90.0f;

	// 描画距離(near, far)
	constexpr float near_distance = 5.0f;
	constexpr float far_distance = 8400.0f;

	// 倒れるアニメーションに何フレーム使うのか
	constexpr float anim_frame_num = 180.0f;

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
	preMousePosX_(0),
	preMousePosY_(0),
	mouseMoveX_(0.0f),
	mouseMoveY_(0.0f),
	rotateDegreeX_(0.0f),
	rotateDegreeY_(0.0f),
	isMouseScreenEdge_(false),
	deadFrame_(0),
	distance_(VGet(0, 0, 0)),
	upVector_(VGet(0, 0, 0)),
	size_(0.0f)
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

	// カメラが向いているY座標の角度の初期化
	rotateDegreeY_ = 180;

	// カメラからどれだけ離れたところ( Near )から、 どこまで( Far )のものを描画するかを設定
	SetCameraNearFar(near_distance, far_distance);;

	// カメラの位置、どこを見ているかを設定する
	SetCameraPositionAndTargetAndUpVec(cameraPos_, cameraInitTarget_, VGet(0, 1, 0));

	// カメラの視野角を設定(ラジアン)
	SetupCamera_Perspective(perspective * DX_PI_F / 180.0f);

}

void Camera::Update(const InputState& input)
{
	if (!pPlayer_->GetIsDead())
	{
		// マウスの座標の取得
		GetMousePoint(&mousePosX_, &mousePosY_);
	}
	if (isMouseScreenEdge_)
	{
		// マウスが画面端についた場合は過去座標を使い、カメラ注視点を動かさない
		mouseMoveX_ = static_cast<float>((preMousePosX_ - preMousePosX_) * 360 / Game::screen_width);
		mouseMoveY_ = static_cast<float>((preMousePosY_ - preMousePosY_) * 360 / Game::screen_height);
		isMouseScreenEdge_ = false;
	}
	else
	{
		// マウスをどのくらい動かしたのか
		mouseMoveX_ = static_cast<float>((mousePosX_ - preMousePosX_) * 360 / Game::screen_width);
		mouseMoveY_ = static_cast<float>((mousePosY_ - preMousePosY_) * 360 / Game::screen_height);
	}

	// よくわからない
	rotateDegreeX_ -= mouseMoveX_;
	rotateDegreeY_ -= mouseMoveY_;

	// 回転角度の制限
	if (rotateDegreeY_ >= 60) rotateDegreeY_ = 60;
	if (rotateDegreeY_ < -30) rotateDegreeY_ = -30;

	// Y軸のカメラの追従
	VECTOR cameraTrans = pPlayer_->GetPos();
	//	cameraTrans.y = 0.0f;							// Y軸カメラの追従を行わない
	//	cameraTrans.y = pPlayer_->GetPos().y * 0.65f;	// Y軸カメラの追従を少し遅くする
	cameraTrans.y = pPlayer_->GetPos().y;				// Y軸カメラの追従をプレイヤーの位置に合わせる

	// 平行行列の作成(なにこれ??)
	MATRIX playerTransMtx = MGetTranslate(cameraTrans);

	// プレイヤーの回転に合わせてカメラの位置、注視点を回転させる
	// プレイヤーの回転情報と平行移動情報を合成
	MATRIX cameraRotMtxSide = MGetRotY(-(DX_PI_F * rotateDegreeX_ / 180));		// 横移動情報の作成
	MATRIX cameraRotMtxVertical = MGetRotX(-(DX_PI_F * rotateDegreeY_ / 180));	// 縦移動情報の作成
	MATRIX cameraRotMtx = MMult(MTranspose(cameraRotMtxVertical), cameraRotMtxSide);
	MATRIX cameraMtxPos = MMult(cameraRotMtxSide, playerTransMtx);
	MATRIX cameraMtxTarget = MMult(cameraRotMtx, playerTransMtx);
	cameraPos_ = VTransform(cameraInitPos_, cameraMtxPos);

	if (!pPlayer_->GetIsDead())
	{
		cameraTarget_ = VTransform(cameraInitTarget_, cameraMtxTarget);
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
		float ratio = static_cast<float>(deadFrame_ / anim_frame_num); // 0～100
		float ratio2 = 1.0f - ratio;								   // 100～0	

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

	// マウスカーソルが画面端近くに近づいた場合初期位置に戻す
	if (mousePosX_ >= Game::screen_width || mousePosX_ <= 0 ||
		mousePosY_ >= Game::screen_height || mousePosY_ <= 0)
	{
		// マウスポインタの初期位置の設定
		SetMousePoint(Game::screen_width / 2, Game::screen_height / 2);
		isMouseScreenEdge_ = true;
	}

	// 座標の保存
	preMousePosX_ = mousePosX_;
	preMousePosY_ = mousePosY_;
}

float Camera::GetCameraAngle() const
{
	return -(DX_PI_F * rotateDegreeX_ / 180);
}
