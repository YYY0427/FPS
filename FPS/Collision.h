#pragma once
#include <DxLib.h>
#include <memory>

class StageManager;
class EnemyManager; 
class Tower;
class ObstacleManager;
class Player;

class Collision
{
public:
	Collision(StageManager* pStages, std::shared_ptr<Tower> pTower, std::shared_ptr<EnemyManager> pEnemyManager, std::shared_ptr<ObstacleManager> pObstacleManager, std::shared_ptr<Player> pPlayer);
	virtual ~Collision();

	// ������
	void Init();

	// �|���S���Ɠ������Ă��邩�`�F�b�N���ĕǃ|���S�������|���S��������
	void CollCheck(int modelHandle, int objectModelHandle, int collisionFrameIndex, VECTOR pos, VECTOR vec, float collisionRadius, int chara);

	// �ǃ|���S�������蔻�菈��
	void WallPolyColCheckProcess(bool isMove, VECTOR vec);

	// ���|���S�������蔻�菈��
	void FloorPolyColCheckProcess(bool isJump, bool isUseGravity, int chara);

	// �W�����v���̏ꍇ�̏��|���S���Ƃ̓����蔻��
	void JumpingFloorPolyColCheckProcess();

	// �����蔻�菈��
	VECTOR MovingColision(int modelHandle, bool isMove, bool isJump, bool isUseGravity, VECTOR pos, VECTOR vec, int chara, float collisionRadius);

//	void HitCollision();

	float GetGroundY() const { return groundY_; }

public:
	enum Chara
	{
		player,
		enemy,
		bee,
		tower
	};

private:
	StageManager* pStages_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<ObstacleManager> pObstacleManager_;
	std::shared_ptr<Player> pPlayer_;

	MV1_COLL_RESULT_POLY* poly_{};
	HITRESULT_LINE lineRes_{};
	MV1_REF_POLYGONLIST refPoly_{};
	MV1_COLL_RESULT_POLY_DIM hitDim_{};

	// �|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* yuka_[2048]{};

	// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* kabe_[2048]{};

	// �ړ���̍��W
	VECTOR moveAfterPos_;

	// �ړ��O�̍��W
	VECTOR oldPos_;

	// �|���S���Ɠ���������
	bool isHitFlag_;

	// �|���S����
	int yukaNum_;
	int kabeNum_;

	float groundY_;
};