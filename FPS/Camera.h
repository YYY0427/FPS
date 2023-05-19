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
	void Draw();

	// プレイヤーのポインタの取得
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// カメラのアングルの取得
	float GetCameraAngle() const { return mouseMoveX_ * 0.01f; }
private:
	std::shared_ptr<Player> pPlayer_ = nullptr;
	
	VECTOR cameraPos_ = VGet(0, 0, 0);
	VECTOR cameraInitPos_ = VGet(0, 0, 0);
	float cameraAngle_ = 0.0f;

	// FPSかどうか
	bool perspectiveFps_ = true;

	// プレイヤーの向いている方向
	float angle_ = 0.0f;

	int mousePosX_;
	int mousePosY_;

	float mouseMoveX_;
	float mouseMoveY_;

};