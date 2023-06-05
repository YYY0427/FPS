#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;

class Enemy
{
public:
	Enemy(const char* fileName, std::shared_ptr<Player> pPlayer);
	virtual ~Enemy();

	void Init();
	void Update();
	void Draw();

	// 2D��UI�̕\��
	void DrawUI();	

	// ���f���̃n���h���擾
	int GetModelHandle() const;

	// �����蔻��̃t���[���C���f�b�N�X�擾
	int GetColFrameIndex() const;

	// �ʒu�擾
	VECTOR GetPos() const { return pos_; }

	// ���S���̎擾
	bool GetDead() const { return isDead_; }

	// �����蔻��̔��a
	float GetColRadius();

	// �_���[�W���󂯂�
	void OnDamage(int damage);

private:
	// �v���C���[�����ʂɂ��邩�ǂ���
	bool IsPlayerFront() const;

	// �v���C���[�Ɍ�����
	void UpdateToPlayer();

	// �O�Ɉړ�����(�v���C���[�������Ă��Ȃ�)
	void UpdateToFront();

	// �����]�����s��(�v���C���[�������Ă��Ȃ�)
	void UpdateTurn();

	// �e�����������Ƃ��̃A�j���[�V����
	void UpdateHitDamage();

	// ���S�A�j���[�V����
	void UpdateDead();

private:
	// �|�C���^
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Model> pModel_;

	// �����o�[�֐��|�C���^
	void(Enemy::* m_updateFunc)();

	// �Đ����Ă���A�j���[�V�����ԍ�
	int animNo_;

	// �t���[���J�E���g
	int frameCount_;

	// �ʒu
	VECTOR pos_;

	// �����Ă������
	float angle_;

	// ��]���x
	float rotSpeed_;

	// HP
	int hp_;

	// ���G����
	int damageFrame_;

	// ����ł��邩
	bool isDead_;

	const char* fileName_;
};