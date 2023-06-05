#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class InputState;

class Camera
{
public:
	Camera();
	virtual ~Camera();

	void Init();
	void Update(const InputState& input);

	// プレイヤーのポインタの取得
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// カメラのアングルの取得
	float GetCameraAngle() const;

	// カメラの注視点の取得
	VECTOR GetCameraTarget() const { return cameraTarget_; }
private:
	std::shared_ptr<Player> pPlayer_ = nullptr;
	
	VECTOR cameraPos_ = VGet(0, 0, 0);
	VECTOR cameraInitPos_ = VGet(0, 0, 0);
	VECTOR cameraTarget_ = VGet(0, 0, 0);

	// カメラ角度
	float cameraAngle_ = 0.0f;

	// FPSかどうか
	bool perspectiveFps_ = true;

	// プレイヤーの向いている方向
	float angle_ = 0.0f;

	// マウスの現在位置
	int mousePosX_ = 0;
	int mousePosY_ = 0;

	// 1フレーム前のマウスの位置
	int preMousePosX_ = 0;
	int preMousePosY_ = 0;

	// マウスの移動量
	float mouseMoveX_ = 0.0f;
	float mouseMoveY_ = 0.0f;

	// 回転角度
	float rotateDegreeX_ = 0.0f;
	float rotateDegreeY_ = 0.0f;

	// マウスが画面端についたか
	bool isMouseScreenEdge_ = false;
};