#pragma once
#include <DxLib.h>
#include <memory>

class Player;
class Camera;

class Shot
{
public:
	Shot();
	virtual ~Shot();

	void Init(int handle);
	void Update();
	void Draw();

	// ���f���̃��[�h�̃Q�b�^�[
	int LoadModel() const;

	// �e�����݂��Ă��邩�̃Q�b�^�[
	bool isExist() const { return isExsit_; }

	// ����
	void Start(VECTOR pos, VECTOR vec);

	// �V���b�g�̒��S�ʒu�̃Q�b�^�[
	VECTOR GetPos() const { return pos_; };

	// �O�̃t���[���̈ʒu���̃Q�b�^�[
	VECTOR GetLastPos() const { return lastPos_; };

	// �e�������Z�b�^�[
	void SetExsit(bool isExsit) { isExsit_ = isExsit; }

	// �v���C���[�̃|�C���^�̃Z�b�^�[
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// �J�����̃|�C���^�̃Z�b�^�[
	void SetCamera(std::shared_ptr<Camera> pCamera) { pCamera_ = pCamera; }
private:
	// �|�C���^
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Camera> pCamera_;

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