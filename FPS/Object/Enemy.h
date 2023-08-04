#pragma once
#include "EnemyBase.h"

class Enemy : public EnemyBase
{
public:
	Enemy(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, MainScene* pMainScene, VECTOR pos, bool isMove, int handle);
	virtual ~Enemy();

	void Update();

	// �_���[�W���󂯂�
	void OnDamage(int damage);

	int GetEnemyType() const { return enemy; }

private:
	// �ǐՏ���
	void Tracking(VECTOR pos, int target, float attackDistance);

	// �U������
	void Attacking(VECTOR pos, int target, float attacDistance);

	// �ҋ@
	void UpdateToIdle();

	// �v���C���[��ǐ�
	void UpdateTrackingToPlayer();

	// �^���[��ǐ�
	void UpdateTrackingToTower();

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

	// �G���U�����Ă��������x�U������܂ł̑ҋ@
	void UpdateAttackWaitTimeToPlayer();
	void UpdateAttackWaitTimeToTower();
	void WaitTime(int target, VECTOR pos, float attacDistance);

private:
	// �����o�[�֐��|�C���^
	void(Enemy::* updateFunc_)();

	bool isPass_ = false; 
};