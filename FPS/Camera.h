#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class InputState;
class Tower;

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

	void SetTower(std::shared_ptr<Tower> pTower) { pTower_ = pTower; }

	// �J�����̃A���O���̎擾
	float GetCameraYaw() const;
	float GetCameraPitch() const;

	VECTOR GetInitTarget() const { return cameraInitTarget_; }

	// �J�����̒����_�̎擾
	VECTOR GetCameraTarget() const { return cameraTarget_; }

	// �}�E�X�̈ʒu�̎擾
	int GetMousePosX() const { return mousePosX_; }
	int GetMousePosY() const { return mousePosY_; }

	// ��ʂ�h�炷
	void Quake();

	// ��ʂ̗h�����Z�b�g
	void SetQuake(int timer, VECTOR quake) { quakeTimer_ = timer, quake_ = quake; }

private:
	// �|�C���^
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Tower> pTower_;

	// �J����
	VECTOR cameraPos_;
	VECTOR cameraInitPos_;
	VECTOR cameraTarget_;
	VECTOR cameraInitTarget_;

	// FPS���ǂ���
	bool perspectiveFps_;

	// �}�E�X�̌��݈ʒu
	int mousePosX_;
	int mousePosY_;

	// �}�E�X�̈ړ���
	float mouseMoveX_;
	float mouseMoveY_;

	// ��]�p�x
	float cameraYaw_;
	float cameraPitch_;

	// �}�E�X����ʒ[�ɂ�����
	bool isMouseScreenEdge_;

	// ����ł���o�߂����t���[����
	int deadFrame_;

	// ��񂵂��ʂ�Ȃ��p
	bool isPass_ = false;

	VECTOR distance_;

	// 
	VECTOR upVector_;

	// �x�N�g���̑傫��
	float size_;

	// ��ʗh��
	int quakeTimer_;
	VECTOR quake_;
};