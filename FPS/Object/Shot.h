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

	void Init();
	void Update();
	void Draw();

	// �e�����݂��Ă��邩
	bool isExist() const { return m_isExsit; }

	// ����
	void Start(VECTOR pos, VECTOR vec);

	// �V���b�g�̒��S�ʒu�擾
	VECTOR GetPos() const { return m_pos; };

	// �O�̃t���[���̈ʒu���擾
	VECTOR GetLastPos() const { return m_lastPos; };

	// �e������
	void SetExsit(bool isExsit) { m_isExsit = isExsit; }

	// �v���C���[�̃|�C���^�̎擾
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// �J�����̃|�C���^�̎擾
	void SetCamera(std::shared_ptr<Camera> pCamera) { pCamera_ = pCamera; }
private:
	// �|�C���^
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Camera> pCamera_;

	// ���݂���
	bool m_isExsit = false;

	// ���݈ʒu
	VECTOR m_pos;

	// 1�t���[���O�̈ʒu
	VECTOR m_lastPos;

	// �ړ�����
	VECTOR m_vec;

	int handle_ = -1;
};