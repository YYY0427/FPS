#pragma once
#include "EnemyBase.h"

class Enemy : public EnemyBase
{
public:
	Enemy(const char* fileName);
	virtual ~Enemy();

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
	void(Enemy::* updateFunc_)();
};