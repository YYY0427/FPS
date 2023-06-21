#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;

class SkyDoom
{
public:
	SkyDoom();
	virtual ~SkyDoom();

	void Init();
	void Update();
	void Draw();

	// �v���C���[�|�C���^�̃Z�b�^�[
	void SetPlayer(std::shared_ptr<Player> pPlayer) { pPlayer_ = pPlayer; }

private:
	// �|�C���^
	std::shared_ptr<Model> pSkydoom_;
	std::shared_ptr<Model> pCloud_;
	std::shared_ptr<Model> pCloud2_;
	std::shared_ptr<Player> pPlayer_;

	// �v���C���[�̈ʒu
	VECTOR playerPos_;

	// ��]��
	float rot_;
};