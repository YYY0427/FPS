#pragma once
#include <DxLib.h>
#include <memory>
#include <vector>
#include "../Model.h"

class InputState;
class Model;
class Camera;
class MainScene;
class Collision;
class Tower;

class Player
{
public:
	Player(MainScene* pMainScene);
	virtual ~Player();

	void Update(const InputState& input);
	void Draw();

	void SetRespawn();

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

	void SetJump(bool jump) { isJump_ = jump; }

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
	MainScene* pMainScene_;

	std::shared_ptr<Collision> pCollision_;

	// �v���C���[�̃��f��
	std::shared_ptr<Model> pModel_;

	std::shared_ptr<Camera> pCamera_;

	std::shared_ptr<Tower> pTower_;

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

	// �n�ʂ��痎������
	bool isFall_;
};