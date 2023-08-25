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
class ObstacleManager;
class BomManager;
class UI;
class InputState;

/// <summary>
/// ���C���V�[��
/// <summary>
class MainScene : public Scene
{
public:
	MainScene(SceneManager& manager, StageManager* pStageManager);
	virtual ~MainScene();

	void Update(const InputState& input);
	void Draw(const InputState& input);

	// �V���b�g������
	void StartPlayerShot(VECTOR pos, VECTOR vec);

	// ���e�B�N���̈ʒu�̎擾
	float GetReticlePosX() const;
	float GetReticlePosY() const;

	// �v���C���[���X�e�[�W���痎�����Ƃ��̃t�F�[�h
	void PlayerFallFade();

	// �Q�[�����X�^�[�g������
	bool GetIsGameStop() const { return isGameStop_; }
private:
	// �t�F�[�h�C��
	void FadeInUpdate();

	// �t�F�[�h�A�E�g
	void FadeOutUpdate(const InputState& input);

	// ���t���[�������蔻��
	void CollisionUpdate();

	// �Q�[���X�^�[�g���Ƀv���C���[�̃A�N�V������҂A�b�v�f�[�g
	void UserReactionWaitUpdate(const InputState& input);

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
	std::shared_ptr<BomManager> pBomManager_;
	std::shared_ptr<SkyDoom> pSkyDoom_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<Collision> pCollision_;
	std::shared_ptr<EnemyShotFactory> pEnemyShotFactory_;
	std::shared_ptr<ObstacleManager> pObstacleManager_;
	std::shared_ptr<UI> pUI_;
	StageManager* pStageManager_;	 // �V�[�����Ȃ��Ȃ��Ă������Ăق����Ȃ��̂�shard_ptr���g��Ȃ�

	// �摜�n���h��
	int gameOverUIhandle_;
	int gameClearUIhandle_;
	int towerIconHandle_;
	int playerIconHandle_;
	int gunUIhandle_;
	int bomUIhandle_;
	int infinityHandle_;
	int completeHandle_;
	int playerDamageUIHandle_;
	int questUIHandle_;

	// �t�H���g�n���h��
	int bulletCounFontHandle_;
	int towerNameFontHandle_;
	int towerNameShadowFontHandle_;
	int playerNameFontHandle_;
	int playerNameShadowFontHandle_;
	int gameClearFontHandle_;

	// �t�F�C�h
	int fadeTimer_;
	int fadeValue_;
	int gameOverUIfadeValue_;
	int gameClearUIFadeValue_;
	int playerDamageUIFadeTimer_;
	int playerDamageUIFadeValue_;
	int questUIfadeValue_;
	int questUIfadeTimer_;

	VERTEX2DSHADER vert_[6];
	int shaderScreen_;

	int psH_;

	// �V���h�E�}�b�v
	int shadowMap_;

	// �Q�[���I�[�o�[���ǂ���
	bool isGameOver_;

	// �v���C���[�V���b�g�ƓG�ɒe������������
	bool isHit_;

	// �G�ɍU���𓖂Ă��Ƃ��ɕ\�����郌�e�B�N���̃G�t�F�N�g�̕\���̎�������
	int reticleEffectDisplayTime;

	// �Q�[���N���A�摜�̊g�嗦
	double gameClearImgExRate_;

	// �N�G�X�g�˗����̉摜�̊g�嗦
	double questImgExRate_;

	// �Q�[���N���A���Ă���̌o�ߎ���
	int gameClearCount_;

	// �v���C���[���_���[�W���󂯂���
	bool isPlayerDamage_;

	// ��񂵂��ʂ�Ȃ�
	bool isPass_;

	// �Q�[�����X�^�[�g������
	bool isGameStop_;
};