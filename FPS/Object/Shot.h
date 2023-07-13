#pragma once
#include <DxLib.h>
#include <memory>

class Player;
class Camera;
class Model;

class Shot
{
public:
	Shot();
	Shot(int handle);
	virtual ~Shot();

	void Update();
	void Draw();

	// ���f���̃��[�h�̃Q�b�^�[
	int LoadModel() const;

	// �e�����݂��Ă��邩�̃Q�b�^�[
	bool isExist() const { return isEnabled_; }

	// ����
	void Start(VECTOR pos, VECTOR vec, std::shared_ptr<Player> pPlayer, std::shared_ptr<Camera> pCamera);

	// �V���b�g�̒��S�ʒu�̃Q�b�^�[
	VECTOR GetPos() const { return pos_; };

	// �O�̃t���[���̈ʒu���̃Q�b�^�[
	VECTOR GetLastPos() const { return lastPos_; };

	// �����蔻��̔��a�̃Q�b�^�[
	float GetColRadius() const;

	// �e�������Z�b�^�[
	void SetExsit(bool isExsit) { isEnabled_ = isExsit; }

private:
	// �|�C���^
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Camera> pCamera_;
	std::shared_ptr<Model> pModel_;

	// ���݂���
	bool isEnabled_;

	// ���f���̃n���h��
	int handle_;

	// ���݈ʒu
	VECTOR pos_;

	// 1�t���[���O�̈ʒu
	VECTOR lastPos_;

	// �ړ�����
	VECTOR vec_;
};