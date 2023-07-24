#pragma once
#include <DxLib.h>
#include <memory>

class InputState;
class Model;
class Camera;
class MainScene;
class Collision;
class Tower;

class PlayerBase
{
public:
	PlayerBase();
	virtual ~PlayerBase();

	void Update(const InputState& input);
	void Draw();

	// �J�����̃|�C���^�̃Z�b�^�[
	void SetCamera(std::shared_ptr<Camera> pCamera) { pCamera_ = pCamera; }

	// �����蔻��|�C���^�̃Z�b�^�[
	void SetCollision(std::shared_ptr<Collision> pCollision) { pCollision_ = pCollision; }

	// �^���[�̃Z�b�^�[
	void SetTower(std::shared_ptr<Tower> pTower) { pTower_ = pTower; }

	// �v���C���[�������������̃Q�b�^�[
	bool IsFall() const { return isFall_; }

	// �v���C���[�̈ʒu�̃Q�b�^�[
	VECTOR GetPos() const { return pos_; }

	// ���f���n���h���̃Q�b�^�[
	int GetHandle() const;

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

	void SetJump(bool jump) { isJump_ = jump; }

protected:
	// ���C���V�[��
	MainScene* pMainScene_;

	std::shared_ptr<Collision> pCollision_;

	// �v���C���[�̃��f��
	std::shared_ptr<Model> pModel_;

	std::shared_ptr<Camera> pCamera_;

	std::shared_ptr<Tower> pTower_;

	// �Đ����Ă���A�j���[�V�����ԍ�
	int animNo_ = -1;

	// �t���[���J�E���g
	int frameCount_ = 0;

	// �v���C���[�̈ʒu
	VECTOR pos_ = VGet(0, 0, 0);

	// �W�����v�����p�����x
	float jumpAcc_ = 0.0f;

	// HP
	int hp_ = 0;

	// ���G����
	int damageFrame_ = 0;

	// �ړ�����
	bool isMoving_ = false;

	// �v���C���[������ł��邩
	bool isDead_ = false;

	// �V���b�g�������Ă��牽�t���[���o�߂������̃J�E���g
	int shotFrameCount_ = 0;

	// �ړ��x�N�g��
	VECTOR moveVec_ = VGet(0, 0, 0);

	// �W�����v����
	bool isJump_ = false;

	// �n�ʂ��痎������
	bool isFall_ = false;
};
