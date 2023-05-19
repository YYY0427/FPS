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

	// �v���C���[�̃|�C���^�̎擾
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// �J�����̃A���O���̎擾
	float GetCameraAngle() const { return mouseMoveX_ * 0.01f; }
private:
	std::shared_ptr<Player> pPlayer_ = nullptr;
	
	VECTOR cameraPos_ = VGet(0, 0, 0);
	VECTOR cameraInitPos_ = VGet(0, 0, 0);
	float cameraAngle_ = 0.0f;

	// FPS���ǂ���
	bool perspectiveFps_ = true;

	// �v���C���[�̌����Ă������
	float angle_ = 0.0f;

	int mousePosX_;
	int mousePosY_;

	float mouseMoveX_;
	float mouseMoveY_;

};