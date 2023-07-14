#pragma once
#include <DxLib.h>
#include <memory>

class StageManager;
class EnemyManager; 
class Player;
class Tower;

class Collision
{
public:
	Collision(StageManager* pStages, std::shared_ptr<Tower> pTower);
	virtual ~Collision();

	// ������
	void Init();

	// �|���S���Ɠ������Ă��邩�`�F�b�N���ĕǃ|���S�������|���S��������
	void CollCheck(int modelHandle, int objectModelHandle, VECTOR pos, VECTOR vec);

	// �ǃ|���S�������蔻�菈��
	void WallPolyColCheckProcess(bool isMove, VECTOR vec);

	// ���|���S�������蔻�菈��
	void FloorPolyColCheckProcess(bool isJump, int chara);

	// �W�����v���̏ꍇ�̏��|���S���Ƃ̓����蔻��
	void JumpingFloorPolyColCheckProcess();

	// �����蔻�菈��
	VECTOR Colision(int modelHandle, bool isMove, bool isJump, VECTOR pos, VECTOR vec, int chara);

	// �Q�b�^�[
	float GetPlayerMinY() const { return playerMinY_; }

	// �Z�b�^�[
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

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
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Tower> pTower_;

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

	// ���|���S���̍���
	float playerMinY_;
};