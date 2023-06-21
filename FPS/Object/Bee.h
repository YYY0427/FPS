#pragma once
#include "EnemyBase.h"

class Bee : public EnemyBase
{
public:
	Bee(const char* fileName);
	virtual ~Bee();

	void Update();

	// �_���[�W���󂯂�
	void OnDamage(int damage);

private:
	// �v���C���[�Ɍ�����
	void UpdateToPlayer();

	// �O�Ɉړ�����(�v���C���[�������Ă��Ȃ�)
	void UpdateToFront();

	// �U������
	void UpdateToAttack();

	// �����]�����s��(�v���C���[�������Ă��Ȃ�)
	void UpdateTurn();

	// �e�����������Ƃ��̃A�j���[�V����
	void UpdateHitDamage();
private:
	// �����o�[�֐��|�C���^
	void(Bee::* updateFunc_)();
};