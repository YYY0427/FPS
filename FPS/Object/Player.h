#pragma once
#include <DxLib.h>
#include <memory>
#include <vector>
#include "../Model.h"

class InputState;
class Model;
class Camera;
class MainScene;
class Field;

class Player
{
public:
	Player();
	virtual ~Player();

	void Init();
	void Update(const InputState& input);
	void Draw();

	void CollisionField();

	// MainScene�̃|�C���^�̃Z�b�^�[
	void SetMainScene(std::shared_ptr<MainScene>pMainScene) { pMainScene_ = pMainScene; }

	void SetField(std::vector<std::shared_ptr<Field>> pField) { pField_ = pField; }

	// �J�����̃|�C���^�̃Z�b�^�[
	void SetCamera(std::shared_ptr<Camera> pCamera) { pCamera_ = pCamera; }

	// �v���C���[�̈ʒu�̃Q�b�^�[
	VECTOR GetPos() const { return pos_; }

	// ���f���n���h���̃Q�b�^�[
	int GetHandle() const { return pModel_->GetModelHandle(); }

	// �v���C���[��HP�̃Q�b�^�[
	int GetHP() const { return hp_; }

	// �v���C���[������ł��邩�̃Q�b�^�[
	bool GetIsDead() const { return isDead_; }

	// �����蔻��̔��a�̃Q�b�^�[
	float GetColRadius() const;

	// �_���[�W���󂯂�
	void OnDamage(int damage);

	// FPS��TPS�؂�ւ��p�̃t���[���̕\����\��
	void SetVisible(bool visible);

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

	std::vector<std::shared_ptr<Field>> pField_;

	// �|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* yuka_[2048]{};

	// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* kabe_[2048]{};

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

	// �V���b�g�������Ă��牽�t���[���o�߂������̃J�E���g
	int shotFrameCount_;

	// �ړ��x�N�g��
	VECTOR moveVec_;

	// �W�����v����
	bool isJump_;
};