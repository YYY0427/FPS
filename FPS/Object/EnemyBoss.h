#pragma once
#include "EnemyBase.h"
class EnemyBoss : public EnemyBase
{
public:
	EnemyBoss(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory, MainScene* pMainScene, VECTOR pos, bool isMove, int handle);
	virtual ~EnemyBoss();

	void Update();

	// �_���[�W���󂯂�
	void OnDamage(int damage);

	int GetEnemyType() const { return enemyBos; }

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

	void UpdateToGameClear();

	// �G���U�����Ă��������x�U������܂ł̑ҋ@
	void UpdateAttackWaitTimeToPlayer();
	void UpdateAttackWaitTimeToTower();
	void WaitTime(int target, VECTOR pos, float attacDistance);

private:
	// �����o�[�֐��|�C���^
	void(EnemyBoss::* updateFunc_)();

	bool isPass_ = false;
};