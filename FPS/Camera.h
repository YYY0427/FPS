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
	float GetCameraAngleX() const;
	float GetCameraAngleY() const;

	// カメラの注視点の取得
	VECTOR GetCameraTarget() const { return cameraTarget_; }
private:
	std::shared_ptr<Player> pPlayer_;
	
	// カメラ
	VECTOR cameraPos_;
	VECTOR cameraInitPos_;
	VECTOR cameraTarget_;
	VECTOR cameraInitTarget_;

	// FPSかどうか
	bool perspectiveFps_;

	// マウスの現在位置
	int mousePosX_;
	int mousePosY_;

	// 1フレーム前のマウスの位置
	int preMousePosX_;
	int preMousePosY_;

	// マウスの移動量
	float mouseMoveX_;
	float mouseMoveY_;

	// 回転角度
	float rotateDegreeX_;
	float rotateDegreeY_;

	// マウスが画面端についたか
	bool isMouseScreenEdge_;

	// 死んでから経過したフレーム数
	int deadFrame_;

	// 一回しか通らない用
	bool isPass_ = false;

	VECTOR distance_;

	// 
	VECTOR upVector_;

	// ベクトルの大きさ
	float size_;
};