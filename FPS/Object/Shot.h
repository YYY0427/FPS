#pragma once
#include <DxLib.h>
#include <memory>

class Player;

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

	// ���a�̎擾
	float GetRadius() const;

	// �v���C���[�̃|�C���^�̎擾
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

private:
	// �v���C���[�̃|�C���^
	std::shared_ptr<Player> pPlayer_;

	// ���݂���
	bool m_isExsit = false;

	// ���݈ʒu
	VECTOR m_pos;

	// 1�t���[���O�̈ʒu
	VECTOR m_lastPos;

	// �ړ�����
	VECTOR m_vec;
};