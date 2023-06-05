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

	// �J�����p�x
	float cameraAngle_ = 0.0f;

	// FPS���ǂ���
	bool perspectiveFps_ = true;

	// �v���C���[�̌����Ă������
	float angle_ = 0.0f;

	// �}�E�X�̌��݈ʒu
	int mousePosX_ = 0;
	int mousePosY_ = 0;

	// 1�t���[���O�̃}�E�X�̈ʒu
	int preMousePosX_ = 0;
	int preMousePosY_ = 0;

	// �}�E�X�̈ړ���
	float mouseMoveX_ = 0.0f;
	float mouseMoveY_ = 0.0f;

	// ��]�p�x
	float rotateDegreeX_ = 0.0f;
	float rotateDegreeY_ = 0.0f;

	// �}�E�X����ʒ[�ɂ�����
	bool isMouseScreenEdge_ = false;
};