#pragma once
#include <DxLib.h>
#include <memory>
#include "../Model.h"

class InputState;
class Model;
class Camera;
class MainScene;

class Player
{
public:
	Player();
	virtual ~Player();

	void Init();
	void Update(const InputState& input);
	void Draw();

	// MainScene�̃|�C���^�̎擾
	void SetMainScene(std::shared_ptr<MainScene>pMainScene) { pMainScene_ = pMainScene; }

	// �v���C���[�̈ʒu���擾����
	VECTOR GetPos() const { return pos_; }

	// ���f���n���h���̎擾
	int GetHandle() const { return pModel_->GetModelHandle(); }

	// �v���C���[��HP�̎擾
	int GetHP() const { return hp_; }

	// �v���C���[������ł��邩�̎擾
	bool GetIsDead() const { return isDead_; }

	// �����蔻��̔��a
	float GetColRadius();

	// �_���[�W���󂯂�
	void OnDamage(int damage);

	// FPS��TPS�؂�ւ��p�̃t���[���̕\����\��
	void SetVisible(bool visible);

	// �J�����̃|�C���^�̎擾
	void SetCamera(std::shared_ptr<Camera> pCamera) { pCamera_ = pCamera; }

private:
	// �ҋ@
	void UpdateIdle(const InputState& input);

	// �V���b�g������
	void UpdateIdleShot(const InputState& input);

	// ���S�A�j���[�V����
	void UpdateDead(const InputState& input);

	// �_���[�W�A�j���[�V����
	void UpdateOnDamage(const InputState& input);

private:
	// �����o�[�֐��|�C���^
	void(Player::* updateFunc_)(const InputState& input);

	// ���C���V�[��
	std::shared_ptr<MainScene> pMainScene_;

	// �v���C���[�̃��f��
	std::shared_ptr<Model> pModel_;

	std::shared_ptr<Camera> pCamera_;

	// �Đ����Ă���A�j���[�V�����ԍ�
	int animNo_;

	// �t���[���J�E���g
	int frameCount_;

	// �v���C���[�̈ʒu
	VECTOR pos_;

	// �W�����v�����p�����x
	float jumpAcc_;

	// HP
	int hp_;

	// ���G����
	int damageFrame_;

	// �ړ�����
	bool isMoving_;

	// �v���C���[������ł��邩
	bool isDead_;
};