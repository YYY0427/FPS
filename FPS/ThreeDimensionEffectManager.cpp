#include "ThreeDimensionEffectManager.h"
#include "EffekseerForDXLib.h"
#include "DrawFunctions.h"
#include <cassert>

ThreeDimensionEffectManager::ThreeDimensionEffectManager()
{
	imgHandle_ = my::MyLoadGraph("Data/Effect/png.png");
}

ThreeDimensionEffectManager::~ThreeDimensionEffectManager()
{
}

int ThreeDimensionEffectManager::Init()
{
	// �t���X�N���[���E�C���h�E�̐؂�ւ��Ń��\�[�X��������̂�h���B
	// Effekseer���g�p����ꍇ�͕K���ݒ肷��B
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// DX���C�u�����̃f�o�C�X���X�g�������̃R�[���o�b�N��ݒ肷��B
	// �E�C���h�E�ƃt���X�N���[���̐؂�ւ�����������ꍇ�͕K�����s����B
	// �������ADirectX11���g�p����ꍇ�͎��s����K�v�͂Ȃ��B
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// Effekseer������������B
	// �����ɂ͉�ʂɕ\������ő�p�[�e�B�N������ݒ肷��B
	if (Effkseer_Init(8000) == -1)
	{
		return -1;
	}

	LoadEffectFile("explosion3");
	LoadEffectFile("hit");
	LoadEffectFile("gameClear");

	return 0;
}

void ThreeDimensionEffectManager::Update()
{
	// DX���C�u�����̃J������Effekseer�̃J�����𓯊�����B
	Effekseer_Sync3DSetting();

	// Effekseer�ɂ��Đ����̃G�t�F�N�g���X�V����B
	UpdateEffekseer3D();
}

void ThreeDimensionEffectManager::Draw()
{
	// Effekseer�ɂ��Đ����̃G�t�F�N�g��`�悷��B
	DrawEffekseer3D();
	DrawGraph(0, 0, imgHandle_, true);
}

void ThreeDimensionEffectManager::End()
{
	// Effekseer���I������B
	Effkseer_End();
}

int ThreeDimensionEffectManager::LoadEffectFile(const char* fileName)
{
	std::string path = "Data/Effect/";
	path += fileName;
	path += ".efkefc";
	int handle = LoadEffekseerEffect(path.c_str());
	assert(handle != -1);
	effectResourceNameAndHandleTable_[fileName] = handle;
	return handle;
}

void ThreeDimensionEffectManager::PlayEffect(const char* fileName, VECTOR pos, float scale, float speed, VECTOR rot)
{
//	assert(effectResourceNameAndHandleTable_[fileName] != -1);
	effectNameAndHandleTable_[fileName] = PlayEffekseer3DEffect(effectResourceNameAndHandleTable_[fileName]);
	SetSpeedPlayingEffekseer3DEffect(effectNameAndHandleTable_[fileName], speed);
	SetScalePlayingEffekseer3DEffect(effectNameAndHandleTable_[fileName], scale, scale,scale);
	SetRotationPlayingEffekseer3DEffect(effectNameAndHandleTable_[fileName], rot.x, rot.y, rot.z);
	SetPosPlayingEffekseer3DEffect(effectNameAndHandleTable_[fileName], pos.x, pos.y, pos.z);
}

bool ThreeDimensionEffectManager::IsEffectPlaying(const char* fileName)
{
	if (IsEffekseer3DEffectPlaying(effectNameAndHandleTable_[fileName]) == 0)
	{
		return true;
	}
	return false;
}