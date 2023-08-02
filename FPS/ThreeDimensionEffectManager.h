#pragma once
#include <DxLib.h>
#include <string>
#include <unordered_map>

/// <summary>
///  �G�t�F�N�g�̊Ǘ����s���V���O���g���N���X
/// </summary>
class ThreeDimensionEffectManager
{
public:
	~ThreeDimensionEffectManager();

	static ThreeDimensionEffectManager& GetInstance()
	{
		static ThreeDimensionEffectManager instance;
		return instance;
	}

	int Init();
	void Update();
	void Draw();
	void End();

	void PlayEffect(const char* fileName, VECTOR pos, float scale, float speed, VECTOR rot = VGet(0, 0, 0));

	// ����̃G�t�F�N�g���Đ�����
	bool IsEffectPlaying(const char* fileName);

	void EffectAllStop();

private:
	ThreeDimensionEffectManager();
	ThreeDimensionEffectManager(const ThreeDimensionEffectManager&) = delete;
	void operator = (const ThreeDimensionEffectManager&) = delete;

	int LoadEffectFile(const char* fileName);

private:
	std::unordered_map<std::string, int> effectResourceNameAndHandleTable_;
	std::unordered_map<std::string, int> effectNameAndHandleTable_;
	int imgHandle_;
};