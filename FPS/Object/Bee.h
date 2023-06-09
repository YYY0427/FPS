#pragma once
#include "EnemyBase.h"

class Bee : public EnemyBase
{
public:
	Bee(const char* fileName, std::shared_ptr<Player> pPlayer);
	virtual ~Bee();

	void Update();

	// 2D��UI�̕\��
	void DrawUI();

	// �_���[�W���󂯂�
	void OnDamage(int damage);

	// �����蔻��̔��a�̃Q�b�^�[
	float GetColRadius() const;

private:
	// �v���C���[�����ʂɂ��邩�ǂ���
	bool IsPlayerFront() const;

	// �v���C���[�Ɍ�����
	void UpdateToPlayer();

	// �O�Ɉړ�����(�v���C���[�������Ă��Ȃ�)
	void UpdateToFront();

	// �����]�����s��(�v���C���[�������Ă��Ȃ�)
	void UpdateTurn();

	// �e�����������Ƃ��̃A�j���[�V����
	void UpdateHitDamage();

	// ���S�A�j���[�V����
	void UpdateDead();

private:
	// �����o�[�֐��|�C���^
	void(Bee::* updateFunc_)();
};