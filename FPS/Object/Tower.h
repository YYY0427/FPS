#pragma once
#include <memory>
#include <DxLib.h>
#include "../Model.h"

class Model;
class Collision;
class EnemyManager;

class Tower
{
public:
	Tower();
	virtual ~Tower();

	void Init();
	void Update();
	void Draw();

	void OnDamage(int damage);

	// �ړI�n�Ɍ���������
	void HeadToDestination(VECTOR checkPoint);

	// �G���t�B�[���h�ɑ��݂��邩
	bool IsEnemyExists();

	// ���݂ǂ̃`�F�b�N�|�C���g��ڎw�����ݒ�
	void CheckPointSet();

	// �Z�b�^�[
	void SetCollision(Collision* pCollision) { pCollision_ = pCollision; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }

	// �Q�b�^�[
	int GetModelHandle() const { return pModel_->GetModelHandle(); }
	VECTOR GetPos() const { return pos_; }
	float GetColRadius() const { return colRadius_; }
	int GetIsDead() const { return isDead_; }
	bool GetIsGoal() const { return isGoal_; }

private:	
	enum
	{
		k_check_point1 = 1,
		k_check_point2,
		k_check_point3,
		k_check_point4,
		k_check_point5,
	};

private:
	// �����o�[�֐��|�C���^
//	void(Tower::* updateFunc_)();

	// �|�C���^
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	Collision* pCollision_;

	// �ʒu
	VECTOR pos_;

	// �ړ��x�N�g��
	VECTOR vec_;

	// ���ݖڎw���Ă���`�F�b�N�|�C���g
	VECTOR checkPointNow_;

	// 
	int checkPoint_;

	// �S�[��������
	bool isGoal_;

	// ���݂�HP
	int hp_;

	// �_���[�W���󂯂Ă��牽�t���[���o������
	int damageFrame_;

	// �����蔻��̔��a
	float colRadius_;

	// ���񂾂�
	bool isDead_;

	// �p�x(�����Ă������)
	float angle_;

	// �����Ă��邩
	bool isMove_;
};