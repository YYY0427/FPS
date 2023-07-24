#pragma once
#include <DxLib.h>
#include <memory>
#include <vector>

class Bom;

class BomManager
{
public:
	BomManager();
	virtual ~BomManager();

	void Update();
	void Draw();

	void StartBom(VECTOR pos, VECTOR vec, float angle);

	std::vector<std::shared_ptr<Bom>> GetBoms() const { return pBoms_; }

private:
	std::vector<std::shared_ptr<Bom>> pBoms_;
};