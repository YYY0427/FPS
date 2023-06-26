#pragma once
#include <vector>
#include <memory>

class Field;

class FieldManager
{
public:
	FieldManager();
	virtual ~FieldManager();

	void Init();
	void Update();
	void Draw();
	std::vector<std::shared_ptr<Field>>& GetField() { return pField_; }
private:
	std::vector<std::shared_ptr<Field>> pField_;
};