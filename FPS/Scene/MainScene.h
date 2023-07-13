#pragma once
#include "Scene.h"
#include <memory>
#include <vector>
#include <DxLib.h>

class Camera;
class Player;
class Shot;
class EnemyManager;
class SkyDoom;
class Tower;
class Collision;
class StageManager;
class EnemyShotFactory;

/// <summary>
/// ���C���V�[��
/// </summary>
class MainScene : public Scene
{
public:
	MainScene(SceneManager& manager, StageManager* pStageManager);
	virtual ~MainScene();

	void Update(const InputState& input);
	void Draw();

	// �V���b�g������
	void StartPlayerShot(VECTOR pos, VECTOR vec);

	// ���e�B�N���̈ʒu�̎擾
	float GetReticlePosX() const;
	float GetReticlePosY() const;

	// �v���C���[���X�e�[�W���痎�����Ƃ��̃t�F�[�h
	void PlayerFallFade();
private:
	// �t�F�[�h�A�E�g
	void FadeOutUpdate(const InputState& input);

	// �t�F�[�h�C��
	void FadeInUpdate();

	// �ʏ�̃A�b�v�f�[�g
	void NormalUpdate(const InputState& input);

private:
	// �����o�֐��|�C���^
	using UpdateFunc_t = void (MainScene::*) (const InputState& input);
	UpdateFunc_t updateFunc_;

	// �|�C���^
	std::shared_ptr<Camera> pCamera_ ;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::vector<std::shared_ptr<Shot>> pShot_;
	std::shared_ptr<SkyDoom> pSkyDoom_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<Collision> pCollision_;
	std::shared_ptr<EnemyShotFactory> pEnemyShotFactory_;
	StageManager* pStageManager_;	 // �V�[�����Ȃ��Ȃ��Ă������Ăق����Ȃ��̂�shard_ptr���g��Ȃ�

	// �摜�n���h��
	int gameOverUIhandle_;
	int gameClearUIhandle_;

	// �t�F�C�h
	int fadeTimer_;
	int fadeValue_;
	int gameOverUIfadeValue_;
	int gameOverUIFadeTimer_;

	// �V���h�E�}�b�v
	int shadowMap_;

	// �Q�[���I�[�o�[���ǂ���
	bool isGameOver_;
};