#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class InputState;
class MainScene;

class Camera
{
public:
	Camera(std::shared_ptr<Player> pPlayer, MainScene* pMainScene);
	virtual ~Camera();

	void Update(const InputState& input);
	void Draw();

	// カメラのアングルの取得
	float GetCameraYaw() const;
	float GetCameraPitch() const;

	VECTOR GetInitTarget() const { return cameraInitTarget_; }

	// カメラの注視点の取得
	VECTOR GetCameraTarget() const { return cameraTarget_; }

	// マウスの位置の取得
	int GetMousePosX() const { return mousePosX_; }
	int GetMousePosY() const { return mousePosY_; }

	// 画面を揺らす
	void Quake();

	// 画面の揺れ具合をセット
	void SetQuake(int timer, VECTOR quake) { quakeTimer_ = timer, quake_ = quake; }

private:
	// ポインタ
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<MainScene> pMainScene_;

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

	// マウスの移動量
	float mouseMoveX_;
	float mouseMoveY_;

	// 回転角度
	float cameraYaw_;
	float cameraPitch_;

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

	// 画面揺れ
	int quakeTimer_;
	VECTOR quake_;
};