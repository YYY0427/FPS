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
	float GetCameraAngle() const;

	// �J�����̒����_�̎擾
	VECTOR GetCameraTarget() const { return cameraTarget_; }
private:
	std::shared_ptr<Player> pPlayer_ = nullptr;
	
	VECTOR cameraPos_ = VGet(0, 0, 0);
	VECTOR cameraInitPos_ = VGet(0, 0, 0);
	VECTOR cameraTarget_ = VGet(0, 0, 0);
	VECTOR reticlePos_ = VGet(0, 0, 0);
	float cameraAngle_ = 0.0f;

	// FPS���ǂ���
	bool perspectiveFps_ = true;

	// �v���C���[�̌����Ă������
	float angle_ = 0.0f;

	int mousePosX_ = 0;
	int mousePosY_ = 0;

	float mouseMoveX_ = 0.0f;
	float mouseMoveY_ = 0.0f;

};