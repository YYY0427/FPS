#pragma once
#include <DxLib.h>

/// <summary>
///  �G�t�F�N�g�̊Ǘ����s���V���O���g���N���X
/// </summary>
class EffectManager
{
public:
	~EffectManager();

	static EffectManager& GetInstance()
	{
		static EffectManager instance;
		return instance;
	}

	int Init();
	void Update();
	void Draw();
	void End();

	void PlayEffect(const char* fileName, VECTOR pos, float scale, float speed, );

private:
	EffectManager();
	EffectManager(const EffectManager&) = delete;
	void operator = (const EffectManager&) = delete;

	int LoadEffectFile(const char* fileName);
};