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
	std::shared_ptr<Player> pPlayer_;
	
	// �J����
	VECTOR cameraPos_;
	VECTOR cameraInitPos_;
	VECTOR cameraTarget_;
	VECTOR cameraInitTarget_;

	// �J�����p�x
	float cameraAngle_;

	// FPS���ǂ���
	bool perspectiveFps_;

	// �v���C���[�̌����Ă������
	float angle_;

	// �}�E�X�̌��݈ʒu
	int mousePosX_;
	int mousePosY_;

	// 1�t���[���O�̃}�E�X�̈ʒu
	int preMousePosX_;
	int preMousePosY_;

	// �}�E�X�̈ړ���
	float mouseMoveX_;
	float mouseMoveY_;

	// ��]�p�x
	float rotateDegreeX_;
	float rotateDegreeY_;

	// �}�E�X����ʒ[�ɂ�����
	bool isMouseScreenEdge_;

	// ����ł���o�߂����t���[����
	int deadFrame_;

	// ��񂵂��ʂ�Ȃ��p
	bool isPass_ = false;

	VECTOR distance_;
	VECTOR upVector_;
	float size_;
};