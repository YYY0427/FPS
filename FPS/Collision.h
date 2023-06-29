#pragma once
#include <DxLib.h>
#include <memory>

class FieldManager;
class EnemyManager;
class Player;
class Tower;

class Collision
{
public:
	Collision();
	virtual ~Collision();

	void Init();
	VECTOR ColisionToField(int modelHandle, bool isMove, bool isJump, VECTOR pos, VECTOR vec);

	float GetMaxY() const { return maxY_; }

	void SetFieldManager(std::shared_ptr<FieldManager> pFieldManager) { pFieldManager_ = pFieldManager; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> pEnemyManager) { pEnemyManager_ = pEnemyManager; }
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }
	void SetTower(std::shared_ptr<Tower> pTower) { pTower_ = pTower; }

private:
	std::shared_ptr<FieldManager> pFieldManager_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Tower> pTower_;

	// �|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* yuka_[2048]{};

	// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* kabe_[2048]{};

	VECTOR moveAfterPos_;

	float maxY_;
};