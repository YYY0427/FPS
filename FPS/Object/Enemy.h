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
	// �ǐՏ���
	void Tracking(VECTOR pos, int target, int attackDistance);

	// �U������
	void Attacking(VECTOR pos, int target, int attacDistance);

	// �v���C���[��ǐ�
	void UpdateToPlayer();

	// �^���[��ǐ�
	void UpdateToTower();

	// �v���C���[�ɍU������
	void UpdateAttackToPlayer();

	// �^���[�ɍU������
	void UpdateAttackToTower();

	// �O�Ɉړ�����(�v���C���[�������Ă��Ȃ�)
	void UpdateToFront();

	// �����]�����s��(�v���C���[�������Ă��Ȃ�)
	void UpdateTurn();

	// �e�����������Ƃ��̃A�j���[�V����
	void UpdateHitDamage();

private:
	// �����o�[�֐��|�C���^
	void(Enemy::* updateFunc_)();
};