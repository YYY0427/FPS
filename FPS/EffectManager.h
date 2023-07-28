#pragma once
#include <DxLib.h>

/// <summary>
///  エフェクトの管理を行うシングルトンクラス
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