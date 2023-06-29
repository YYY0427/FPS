#include "Camera.h"
#include "Object/Player.h"
#include "InputState.h"
#include "Game.h"
#include "Object/Tower.h"
#include <DxLib.h>

namespace
{
	// �J�����̏����ʒu�ƃJ�����̏��������_
//	constexpr VECTOR tps_camera_pos{ 0, 500, 300 };
	constexpr VECTOR tps_camera_pos{ 500, 300, -300 };
	constexpr VECTOR tps_camera_target{ 0, 0, 0 };
//	constexpr VECTOR tps_camera_target{ 0, 400, -120 };

	constexpr VECTOR fps_camera_pos{ 0, 200, -10 };
	constexpr VECTOR fps_camera_target{ 0, 90, 500 };

	// �J��������p�̐ݒ�
	constexpr float perspective = 90.0f;

	// �`�拗��(near, far)
	constexpr float near_distance = 5.0f;
	constexpr float far_distance = 18400.0f;

	// �|���A�j���[�V�����ɉ��t���[���g���̂�
	constexpr int anim_frame_num = 180;

	// ���񑬓x
	constexpr float rot_speed = 0.01f;
}

Camera::Camera() :
	cameraPos_(VGet(0, 0, 0)),
	cameraInitPos_(VGet(0, 0, 0)),
	cameraTarget_(VGet(0, 0, 0)),
	cameraInitTarget_(VGet(0, 0, 0)),
	perspectiveFps_(true),
	mousePosX_(0),
	mousePosY_(0),
	mouseMoveX_(0.0f),
	mouseMoveY_(0.0f),
	cameraYaw_(0.0f),
	cameraPitch_(0.0f),
	isMouseScreenEdge_(false),
	deadFrame_(0),
	distance_(VGet(0, 0, 0)),
	upVector_(VGet(0, 0, 0)),
	size_(0.0f),
	quakeTimer_(0),
	quake_(VGet(0, 0, 0))
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

	// �J�����̒����_�̐ݒ�
	cameraInitTarget_ = fps_camera_target;

	// �J��������ǂꂾ�����ꂽ�Ƃ���( Near )����A �ǂ��܂�( Far )�̂��̂�`�悷�邩��ݒ�
	SetCameraNearFar(near_distance, far_distance);;

	// �J�����̈ʒu�A�ǂ������Ă��邩��ݒ肷��
	SetCameraPositionAndTargetAndUpVec(cameraPos_, cameraInitTarget_, VGet(0, 1, 0));

	// �J�����̎���p��ݒ�(���W�A��)
	SetupCamera_Perspective(perspective * DX_PI_F / 180.0f);
}

void Camera::Update(const InputState& input)
{
	// �v���C���[�������Ă���ꍇ�̂݃}�E�X�̍��W�̎擾
	if (!pPlayer_->GetIsDead() && !pTower_->GetIsDead())
	{
		GetMousePoint(&mousePosX_, &mousePosY_);
	}

	// �}�E�X�J�[�\���̈ʒu�̏�����
	SetMousePoint(Game::screen_width / 2, Game::screen_height / 2);

	// �}�E�X���ǂ̂��炢���������̂�
	mouseMoveX_ = static_cast<float>(mousePosX_ - (Game::screen_width / 2));
	mouseMoveY_ = static_cast<float>(mousePosY_ - (Game::screen_height / 2));

	// �J�����̉�]�Ƀ}�E�X�̈ړ��ʂ𑫂�
	cameraYaw_ += mouseMoveX_ * rot_speed;
	cameraPitch_ += mouseMoveY_ * rot_speed;

	// �J�����̉�]�̐���
	if (cameraPitch_ <= 150 * rot_speed)	cameraPitch_ = 150 * rot_speed;
	if (cameraPitch_ >= 350 * rot_speed)	cameraPitch_ = 350 * rot_speed;

	// Y���̃J�����̒Ǐ]
	VECTOR cameraTrans = pPlayer_->GetPos();
	//	cameraTrans.y = 0.0f;							// Y���J�����̒Ǐ]���s��Ȃ�
	//	cameraTrans.y = pPlayer_->GetPos().y * 0.65f;	// Y���J�����̒Ǐ]�������x������

	// ���s�s��̍쐬(�Ȃɂ���??)
	MATRIX playerTransMtx = MGetTranslate(cameraTrans);

	// �v���C���[�̉�]�ɍ��킹�ăJ�����̈ʒu�A�����_����]������
	// �v���C���[�̉�]���ƕ��s�ړ���������
	MATRIX cameraRotMtxSide = MGetRotY(cameraYaw_);				// ���ړ����̍쐬
	MATRIX cameraRotMtxVertical = MGetRotX(-cameraPitch_);		// �c�ړ����̍쐬
	MATRIX cameraRotMtx = MMult(cameraRotMtxVertical, cameraRotMtxSide);
	MATRIX cameraMtxPos = MMult(cameraRotMtxSide, playerTransMtx);
	MATRIX cameraMtxTarget = MMult(cameraRotMtx, playerTransMtx);
	cameraPos_ = VTransform(cameraInitPos_, cameraMtxPos);

	// �����Ă���ꍇ�̂݃v���C���[�ɃJ������Ǐ]
	if (!pPlayer_->GetIsDead())
	{
		cameraTarget_ = VTransform(cameraInitTarget_, cameraMtxTarget);
	//	cameraTarget_ = cameraInitTarget_;
	}
	// �v���C���[������ł�����J���������S���J�����ɐ؂�ւ�
	else if(pPlayer_->GetIsDead() && perspectiveFps_)
	{
		// ��񂵂��ʂ�Ȃ�
		if (!isPass_)
		{
			// �J�����̌��Ă�����W����J�����̈ʒu�������ăx�N�g�������߂�
			distance_ = VSub(cameraTarget_, cameraPos_);

			// �x�N�g�����狗��(�傫��)�����߂�
			size_ = VSize(distance_);

			// �����������������Ɋ���U��
			upVector_ = VGet(0, size_, 0);

			// �ʂ����t���O�𗧂Ă�
			isPass_ = true;
		}

		// �t���[�����Z
		if (deadFrame_++ >= anim_frame_num - 1)
		{
			deadFrame_ = anim_frame_num - 1;
		}

		// �����v�Z
		float ratio = static_cast<float>(deadFrame_) / static_cast<float>(anim_frame_num); // 0�`100
		float ratio2 = 1.0f - ratio;													   // 100�`0	

		// �J�����̌��Ă�������̃x�N�g���ƃJ�����̈ʒu����J�����̌��Ă�����W�����߂�
		cameraTarget_ = VAdd(VAdd(VScale(distance_, ratio2), VScale(upVector_, ratio)), cameraPos_);
	}

	// �J��������ǂꂾ�����ꂽ�Ƃ���( Near )����A �ǂ��܂�( Far )�̂��̂�`�悷�邩��ݒ�
	SetCameraNearFar(near_distance, far_distance);

	// �J�����̎���p��ݒ�(���W�A��)
	SetupCamera_Perspective(perspective * DX_PI_F / 180.0f);

	// �J�����̈ʒu�A�ǂ������Ă��邩��ݒ肷��
	SetCameraPositionAndTargetAndUpVec(cameraPos_, cameraTarget_, VGet(0, 1, 0));
	
	// �J�����؂�ւ�
#ifdef _DEBUG
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

		// �v���C���[�������Ă���ꍇ����̃��f���̔�\��
		if (!pPlayer_->GetIsDead())
		{
			pPlayer_->SetVisible(false);
		}
	}
	else
	{
		// �J�����̈ʒu�̐؂�ւ�
		cameraPos_ = tps_camera_pos;
		cameraInitPos_ = tps_camera_pos;

		// �v���C���[�̓���̃��f���̕\��
		pPlayer_->SetVisible(true);
	}
#endif

	// �J������h�炷
	Quake();
}

void Camera::Draw()
{
	DrawFormatString(10, 180, 0x000000, "mousePos = %d, %d", mousePosX_, mousePosY_);
}

float Camera::GetCameraYaw() const
{
	return cameraYaw_;
}

float Camera::GetCameraPitch() const
{
	return cameraPitch_;
}

void Camera::Quake()
{
	if (quakeTimer_ > 0)
	{
		quake_ = VScale(quake_, -0.95f);

		--quakeTimer_;
	}
	else
	{
		quake_ = VGet(0, 0, 0);
	}

	cameraInitPos_ = VAdd(cameraInitPos_, quake_);
}
