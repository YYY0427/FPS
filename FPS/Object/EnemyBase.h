#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;
class MainScene;

class EnemyBase
{
public:
	EnemyBase();
	virtual ~EnemyBase(){}

	void Init();
	virtual void Update() = 0;
	void Draw();

	// 2D��UI�̕\��
	virtual void DrawUI() = 0;

	// �_���[�W���󂯂�
	virtual void OnDamage(int damage) = 0;

	// ���f���̃n���h���̃Q�b�^�[
	int GetModelHandle() const;

	// �����蔻��̃t���[���C���f�b�N�X�̃Q�b�^�[
	int GetColFrameIndex() const;

	// �ʒu�̃Q�b�^�[
	VECTOR GetPos() const { return pos_; }

	// ���S���̃Q�b�^�[
	bool GetDead() const { return isDead_; }

	// ���C���V�[���̃Z�b�^�[
	void SetMainScene(std::shared_ptr<MainScene> pMainScene) { pMainScene_ = pMainScene; }

	// �v���C���[�̃Z�b�^�[
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

	// �����蔻��̔��a�̃Q�b�^�[
	virtual float GetColRadius() const = 0;

protected:
	// �v���C���[�����ʂɂ��邩�ǂ���
	virtual bool IsPlayerFront() const = 0;

	// �v���C���[�Ɍ�����
	virtual void UpdateToPlayer() = 0;

	// �O�Ɉړ�����(�v���C���[�������Ă��Ȃ�)
	virtual void UpdateToFront() = 0;

	// �����]�����s��(�v���C���[�������Ă��Ȃ�)
	virtual void UpdateTurn() = 0;

	// �e�����������Ƃ��̃A�j���[�V����
	virtual void UpdateHitDamage() = 0;

	// ���S�A�j���[�V����
	virtual void UpdateDead() = 0;

protected:
	// �|�C���^
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<MainScene> pMainScene_;

	// �Đ����Ă���A�j���[�V�����ԍ�
	int animNo_ = 0;

	// �t���[���J�E���g
	int frameCount_ = 0;

	// �ʒu
	VECTOR pos_ = VGet(0, 0, 0);

	// �����Ă������
	float angle_ = 0.0f;

	// ��]���x
	float rotSpeed_ = 0.0f;

	// HP
	int hp_ = 0;

	// ���G����
	int damageFrame_ = 0;

	// ����ł��邩
	bool isDead_ = false;

	// ���f���n���h���̃t�@�C��
	const char* fileName_ = nullptr;
};