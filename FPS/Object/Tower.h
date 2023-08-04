#pragma once
#include <memory>
#include <DxLib.h>
#include "../Model.h"

class Model;
class Collision;
class EnemyManager;
class ObstacleManager;
class StageManager;

class Tower
{
public:
	Tower(StageManager* pStageManager, std::shared_ptr<ObstacleManager> pObstacleManager);
	virtual ~Tower();

	void Update(bool isGameStart);
	void Draw();

	// �_���[�W���󂯂����̏���
	void OnDamage(int damage);

	// �ړI�n�Ɍ���������
	void HeadToDestination(VECTOR checkPoint);

	// ���ʂɏ�Q�������邩�m�F������ꍇ�������~�߂�
	void IsObstacleFront();

	// ���݂ǂ̃`�F�b�N�|�C���g��ڎw�����ݒ�
	void CheckPointSet();

	// �Z�b�^�[
	void SetCollision(std::shared_ptr<Collision> pCollision) { pCollision_ = pCollision; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }
	void SetIsMove(bool isMove) { isMove_ = isMove; }
	
	// �Q�b�^�[
	int GetModelHandle() const { return pModel_->GetModelHandle(); }
	VECTOR GetPos() const { return pos_; }
	float GetColRadius() const { return colRadius_; }
	int GetIsDead() const { return isDead_; }
	bool GetIsGoal() const { return isGoal_; }
	int GetCheckPoint() const { return checkPoint_; }
	int GetHP() const { return hp_; }
	int GetMaxHP() const;
	float GetCollisionFrameIndex() const { return pModel_->GetColFrameIndex(); }

public:	
	enum CheckPoint
	{
		start,
		check_point1,
		check_point2,
		check_point3,
		check_point4,
		check_point5,
		check_point6,
		check_point7,
		check_point8,
		check_point9,
		goal
	};

private:
	// �����o�[�֐��|�C���^
//	void(Tower::* updateFunc_)();

	// �|�C���^
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Collision> pCollision_;
	std::shared_ptr<ObstacleManager> pObstacleManager_;
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
	VECTOR checkPointPos5_;
	VECTOR checkPointPos6_;
	VECTOR checkPointPos7_;
	VECTOR checkPointPos8_;
	VECTOR checkPointPos9_;

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