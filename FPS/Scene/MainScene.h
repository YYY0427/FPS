#pragma once
#include "Scene.h"
#include <memory>
#include <vector>
#include <DxLib.h>

class Camera;
class Player;
class Field;
class Shot;
class EnemyManager;

/// <summary>
/// ���C���V�[��
/// </summary>
class MainScene : public Scene
{
public:
	MainScene(SceneManager& manager);
	virtual ~MainScene();

	void Init();
	void Update(const InputState& input);
	void Draw();

	// �V���b�g������
	void StartShot(VECTOR pos, VECTOR vec);

	// ���e�B�N���̈ʒu�̎擾
	float GetReticlePosX() const;
	float GetReticlePosY() const;

private:
	void FadeInUpdate(const InputState& input);
	void NormalUpdate(const InputState& input);
	void FadeOutUpdate(const InputState& input);

private:
	// �����o�֐��|�C���^
	using UpdateFunc_t = void (MainScene::*) (const InputState& input);
	UpdateFunc_t updateFunc_;

	// �|�C���^
	std::shared_ptr<Camera> pCamera_ ;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<Field> pField_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::vector<std::shared_ptr<Shot>> pShot_;

	// �Q�[���I�[�o�[����UI�p�n���h��
	int gameOverUIhandle_;

	// �t�F�C�h
	int fadeTimer_;
	int fadeValue_;
	int gameOverUIfadeValue_;


	// �V���h�E�}�b�v
	int shadowMap_;
};