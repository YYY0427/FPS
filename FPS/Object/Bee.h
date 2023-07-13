#pragma once
#include "EnemyBase.h"

class Bee : public EnemyBase
{
public:
	Bee(std::shared_ptr<Player> pPlayer, std::shared_ptr<Tower> pTower, std::shared_ptr<Collision> pCollision, std::shared_ptr<EnemyShotFactory> pEnemyShotFactory);
	virtual ~Bee();

	void Update();

	// �_���[�W���󂯂�
	void OnDamage(int damage);
	
private:
	// �ǐՏ���
	void Tracking(VECTOR pos, int target, float attackDistance);

	// �U������
	void Attacking(VECTOR pos, int target, float attacDistance);

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
private:
	// �����o�[�֐��|�C���^
	void(Bee::* updateFunc_)();

	float degree_;

	int cnt_;
};