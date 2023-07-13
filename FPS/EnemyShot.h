#pragma once
#include <DxLib.h>
#include <memory>

class Model;
class Player;

class EnemyShot
{
public:
	EnemyShot(int handle, std::shared_ptr<Player> pPlayer);
	virtual ~EnemyShot();

	void Init();
	void Update();
	void Draw();

	void Start(const VECTOR& pos, const VECTOR& vec);

	bool IsEnabled() const { return isEnabled_; }
private:
	std::shared_ptr<Model> pModel_;
	std::shared_ptr<Player> pPlayer_;

	int handle_;

	VECTOR pos_;
	VECTOR vec_;

	bool isEnabled_;
};