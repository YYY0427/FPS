#pragma once
#include <memory>
#include <DxLib.h>

class Player;
class Model;

class SkyDoom
{
public:
	SkyDoom(std::shared_ptr<Player> pPlayer);
	virtual ~SkyDoom();

	void Update();
	void Draw();

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