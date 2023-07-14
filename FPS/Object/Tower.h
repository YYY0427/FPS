#pragma once
#include <memory>
#include <DxLib.h>
#include "../Model.h"

class Model;
class Collision;
class EnemyManager;
class StageManager;

class Tower
{
public:
	Tower(StageManager* pStageManager);
	virtual ~Tower();

	void Update();
	void Draw();

	void OnDamage(int damage);

	// �ړI�n�Ɍ���������
	void HeadToDestination(VECTOR checkPoint);

	// �G���t�B�[���h�ɑ��݂��邩
	bool IsEnemyEnabled();

	// ���݂ǂ̃`�F�b�N�|�C���g��ڎw�����ݒ�
	void CheckPointSet();

	// �Z�b�^�[
	void SetCollision(std::shared_ptr<Collision> pCollision) { pCollision_ = pCollision; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }
	
	// �Q�b�^�[
	int GetModelHandle() const { return pModel_->GetModelHandle(); }
	VECTOR GetPos() const { return pos_; }
	float GetColRadius() const { return colRadius_; }
	int GetIsDead() const { return isDead_; }
	bool GetIsGoal() const { return isGoal_; }
	int GetCheckPoint() const { return checkPoint_; }

public:	
	enum CheckPoint
	{
		start,
		check_point1,
		check_point2,
		check_point3,
		check_point4,
		goal
	};

private:
	// �����o�[�֐��|�C���^
//	void(Tower::* updateFunc_)();

	// �|�C���^
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Collision> pCollision_;
	StageManager* pStageManager_;

	// �ʒu
	VECTOR pos_;

	// �ړ��x�N�g��
	VECTOR vec_;

	// ���ꂼ��̃`�F�b�N�|�C���g�̈ʒu
	VECTOR checkPointPos1_;
	VECTOR checkPointPos2_;
	VECTOR checkPointPos3_;
	VECTOR checkPointPos4_;

	// ���ݖڎw���Ă���`�F�b�N�|�C���g�̈ʒu
	VECTOR checkPointPos_;
	
	// ���ݖڎw���Ă���`�F�b�N�|�C���g��int�^�ɂ�������
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