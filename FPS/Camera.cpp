#include "Camera.h"
#include "Object/Player.h"
#include "InputState.h"
#include "Game.h"
#include <DxLib.h>

namespace
{
	// �J�����̏����ʒu
	constexpr VECTOR tps_camera_pos{ 0, 720, 620 };
	constexpr VECTOR tps_camera_target{ 0, 400, -120 };
//	constexpr VECTOR fps_camera_pos{ 0, 200, 21 };
	constexpr VECTOR fps_camera_pos{ 0, 200, -10 };
	constexpr VECTOR fps_camera_target{ 0, 90, -500 };

	// �J��������p�̐ݒ�
	constexpr float perspective = 90.0f;

	// ���񑬓x
	constexpr float rot_speed = 0.01f;
}

Camera::Camera()
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
	// �}�E�X�J�[�\���̔�\��
	SetMouseDispFlag(false);
#endif 

	// �}�E�X�|�C���^�̏����ʒu�̐ݒ�
	SetMousePoint(Game::screen_width / 2, Game::screen_height / 2);

	// �v���C���[�̓���̃��f���̔�\��
	pPlayer_->SetVisible(false);

	// �w�肵���ʒu�ɃJ������ݒu
	cameraPos_ = fps_camera_pos;
	cameraInitPos_ = fps_camera_pos;

	// �J�����̌����Ă�������̐ݒ�
	cameraAngle_ = angle_;

	// �J��������ǂꂾ�����ꂽ�Ƃ���( Near )����A �ǂ��܂�( Far )�̂��̂�`�悷�邩��ݒ�
	SetCameraNearFar(5.0f, 2800.0f);

	// �J�����̈ʒu�A�ǂ������Ă��邩��ݒ肷��
	SetCameraPositionAndTargetAndUpVec(cameraPos_, fps_camera_target, VGet(0, 1, 0));

	// �J�����̎���p��ݒ�(���W�A��)
	SetupCamera_Perspective(perspective * DX_PI_F / 180.0f);
}

void Camera::Update(const InputState& input)
{
#if false
	cameraAngle_ = pPlayer_->GetAngle();
#else
	cameraAngle_ = (cameraAngle_ * 0.8f) + (angle_ * 0.2f);
#endif
	// �}�E�X�̍��W�̎擾
	GetMousePoint(&mousePosX_, &mousePosY_);

	if (isMouseScreenEdge_)
	{
		// �}�E�X����ʒ[�ɂ����ꍇ�͉ߋ����W���g���A�J���������_�𓮂����Ȃ�
		mouseMoveX_ = static_cast<float>((preMousePosX_ - preMousePosX_) * 360 / Game::screen_width);
		mouseMoveY_ = static_cast<float>((preMousePosY_- preMousePosY_) * 360 / Game::screen_height);
		isMouseScreenEdge_ = false;
	}
	else
	{
		// �}�E�X���ǂ̂��炢���������̂�
		mouseMoveX_ = static_cast<float>((mousePosX_ - preMousePosX_) * 360 / Game::screen_width);
		mouseMoveY_ = static_cast<float>((mousePosY_ - preMousePosY_) * 360 / Game::screen_height);
	}

	// �悭�킩��Ȃ�
	rotateDegreeX_ -= mouseMoveX_;
	rotateDegreeY_ -= mouseMoveY_;

	// ��]�p�x�̐���
	if (rotateDegreeY_ >= 60) rotateDegreeY_ = 60;
	if (rotateDegreeY_ < -30) rotateDegreeY_ = -30;
	
	// Y���̃J�����̒Ǐ]
	VECTOR cameraTrans = pPlayer_->GetPos();
//	cameraTrans.y = 0.0f;							// Y���J�����̒Ǐ]���s��Ȃ�
//	cameraTrans.y = pPlayer_->GetPos().y * 0.65f;	// Y���J�����̒Ǐ]�������x������
	cameraTrans.y = pPlayer_->GetPos().y;			// Y���J�����̒Ǐ]���v���C���[�̈ʒu�ɍ��킹��

	// ���s�s��̍쐬(�Ȃɂ���??)
	MATRIX playerTransMtx = MGetTranslate(cameraTrans);
	
	// �v���C���[�̉�]�ɍ��킹�ăJ�����̈ʒu�A�����_����]������
	// �v���C���[�̉�]���ƕ��s�ړ���������
	MATRIX cameraRotMtxSide = MGetRotY(-(DX_PI_F * rotateDegreeX_ / 180));		// ���ړ����̍쐬
	MATRIX cameraRotMtxVertical = MGetRotX(-(DX_PI_F * rotateDegreeY_ / 180));	// �c�ړ����̍쐬
	MATRIX cameraRotMtx = MMult(MTranspose(cameraRotMtxVertical), cameraRotMtxSide);
	MATRIX cameraMtxPos = MMult(cameraRotMtxSide, playerTransMtx);
	MATRIX cameraMtxTarget = MMult(cameraRotMtx, playerTransMtx);
	cameraPos_ = VTransform(cameraInitPos_, cameraMtxPos);
	cameraTarget_ = VTransform(fps_camera_target, cameraMtxTarget);

	// �J��������ǂꂾ�����ꂽ�Ƃ���( Near )����A �ǂ��܂�( Far )�̂��̂�`�悷�邩��ݒ�
	SetCameraNearFar(5.0f, 2800.0f);

	// �J�����̎���p��ݒ�(���W�A��)
	SetupCamera_Perspective(perspective * DX_PI_F / 180.0f);

	// �J�����̈ʒu�A�ǂ������Ă��邩��ݒ肷��
	SetCameraPositionAndTargetAndUpVec(cameraPos_, cameraTarget_, VGet(0, 1, 0));
	
	// �J�����؂�ւ�
	if (input.IsTriggered(InputType::perspective))
	{
		if (perspectiveFps_)
			perspectiveFps_ = false;
		else
			perspectiveFps_ = true;
	}
	if (perspectiveFps_)
	{
		// �J�����̈ʒu�̐؂�ւ�
		cameraPos_ = fps_camera_pos;
		cameraInitPos_ = fps_camera_pos;

		// �v���C���[�̓���̃��f���̔�\��
		pPlayer_->SetVisible(false);
	}
	else
	{
		// �J�����̈ʒu�̐؂�ւ�
		cameraPos_ = tps_camera_pos;
		cameraInitPos_ = tps_camera_pos;

		// �v���C���[�̓���̃��f���̕\��
		pPlayer_->SetVisible(true);
	}

	// �}�E�X�J�[�\������ʒ[�߂��ɋ߂Â����ꍇ�����ʒu�ɖ߂�
	if (mousePosX_ >= Game::screen_width || mousePosX_ <= 0 ||
		mousePosY_ >= Game::screen_height || mousePosY_ <= 0)
	{
		// �}�E�X�|�C���^�̏����ʒu�̐ݒ�
		SetMousePoint(Game::screen_width / 2, Game::screen_height / 2);
		isMouseScreenEdge_ = true;
	}

	// ���W�̕ۑ�
	preMousePosX_ = mousePosX_;
	preMousePosY_ = mousePosY_;
}

float Camera::GetCameraAngle() const
{
	return -(DX_PI_F * rotateDegreeX_ / 180);
}
