#pragma once
#include <DxLib.h>
#include <memory>

class Model;
class Tower;

class EnemyShot
{
public:
	EnemyShot();
	virtual ~EnemyShot();

	void Init(int handle);
	void Update();
	void Draw();

	// ���f���̃��[�h�̃Q�b�^�[
	int LoadModel() const;

	void Start(VECTOR pos, VECTOR vec);

	// �����蔻��̔��a�̃Q�b�^�[
	float GetColRadius() const;

	// �e�������Z�b�^�[
	void SetExsit(bool isExsit) { isExsit_ = isExsit; }

	// �v���C���[�̃|�C���^�̃Z�b�^�[
	void SetTower(std::shared_ptr<Tower> pTower) { pTower_ = pTower; }
private:
	// �|�C���^
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::shared_ptr<Tower> pTower_;

	// ���݂���
	bool isExsit_;

	// ���f���̃n���h��
	int handle_;

	// ���݈ʒu
	VECTOR pos_;

	// 1�t���[���O�̈ʒu
	VECTOR lastPos_;

	// �ړ�����
	VECTOR vec_;
};