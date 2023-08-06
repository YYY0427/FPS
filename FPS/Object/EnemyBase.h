#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;
class Tower;
class Collision;
class EnemyShotFactory;
class MainScene;

class EnemyBase
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	virtual void Update() = 0;
	void Draw();

	// �_���[�W���󂯂�
	virtual void OnDamage(int damage) = 0;

	virtual int GetEnemyType() const = 0;

	bool GetIsAttak() const { return isAttack_; }

	void SetIsDead(bool isDead) { isDead_ = isDead; }

	// ���f���̃n���h���̃Q�b�^�[
	int GetModelHandle() const;

	// �����蔻��̃t���[���C���f�b�N�X�̃Q�b�^�[
	int GetColFrameIndex() const;

	// �ʒu�̃Q�b�^�[
	VECTOR GetPos() const { return pos_; }

	// ���S���̃Q�b�^�[
	bool GetDead() const { return isDead_; }

	// �����蔻��̔��a�̃Q�b�^�[
	float GetCollisionRadius() const { return colRadius_; };

	enum EnemyType
	{
		enemy,
		enemyBos,
		bee,
	};

	enum Target
	{
		player,
		tower
	};

protected:
	// �^�[�Q�b�g�����ʂɂ��邩�ǂ���
	bool IsPlayerFront(VECTOR targetPos) const;

	// �^�[�Q�b�g�����m�͈͂ɓ��������ǂ���
	bool IsTargetDetection(VECTOR targetPos, float targetCollisionRadius) const;

	// �v���C���[�Ɍ�����
	virtual void UpdateTrackingToPlayer() = 0;

	// �O�Ɉړ�����(�v���C���[�������Ă��Ȃ�)
	virtual void UpdateToFront() = 0;

	// �����]�����s��(�v���C���[�������Ă��Ȃ�)
	virtual void UpdateTurn() = 0;

	// �e�����������Ƃ��̃A�j���[�V����
	virtual void UpdateHitDamage() = 0;

	// ���S�A�j���[�V����
	virtual void UpdateDead();

protected:
	struct HP
	{
		// HP
		int hp_ = 0;

		// �ő�HP
		int maxHp_ = 0;

		// HP��\������ێw��ʒu����ǂꂾ��Y�����v���X���邩
		float hpUIDrawY_ = 0.0f;
	};

public:
	HP GetHP() const { return sHp_; }

protected:
	// �|�C���^
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<EnemyShotFactory> pEnemyShotFactory_;
	std::shared_ptr<Collision> pCollision_;
	MainScene* pMainScene_;

	HP sHp_;

	// ���ݍĐ����Ă���A�j���[�V�����ԍ�
	int animNo_ = 0;

	// �A�j���[�V�����ԍ�
	int deadAnimNo_ = 0;

	int discoverAnimNo_ = 0;

	// �t���[���J�E���g
	int frameCount_ = 0;

	// �ʒu
	VECTOR pos_ = VGet(0, 0, 0);

	// �v���C���[����̋���
	VECTOR toTargetVec_ = VGet(0, 0, 0);

	// �����Ă������
	float angle_ = 0.0f;

	// ��]���x
	float rotSpeed_ = 0.0f;

	// ���G����
	int damageFrame_ = 0;

	// ����ł��邩
	bool isDead_ = false;

	// ���f���n���h���̃t�@�C��
	const char* fileName_ = nullptr;

	// �����蔻��̔��a
	float colRadius_ = 0.0f;

	// ����p
	float viewAngle_ = 0.0f;

	// �L�����N�^�[�̌����Ă������
	VECTOR dir_ = VGet(0, 0, 0);

	// �G������ł��������܂ł̎���
	int deadDisappearTime_ = 0;

	// �U���̍Ďg�p�܂őҋ@�t���[���J�E���g
	int attackWaitTimer_ = 0;

	// �U�����Ă��邩
	bool isAttack_ = false;

	// ���m�͈�
	float detectionRange_ = 0.0f;

	int handle_ = -1;

	bool targetDiscover_ = false;

	int target_ = 0;
};