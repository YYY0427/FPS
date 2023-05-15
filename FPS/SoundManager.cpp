#include "SoundManager.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	constexpr char sound_file_signature[] = "SND_";
	constexpr char sound_config_file_path[] = "./sound.conf";
	constexpr float sound_config_version = 1.0f;
}

int SoundManager::LoadSoundFile(const char* fileName, const char* ext)
{
	std::string path = "Data/sound/SE/";
	path += fileName;
	path += ext;
	int handle = LoadSoundMem(path.c_str());
	assert(handle >= 0);
	nameAndHandleTable_[fileName] = handle;
	return handle;
}

SoundManager::SoundManager()
{
	LoadSoundConfig();
	SetSEVolume(volumeSE_);
	SetBGMVolume(volumeBGM_);
}

void SoundManager::LoadSoundConfig()
{
	SoundConfigInfo conf = {};
	FILE* fp = nullptr;
	fopen_s(&fp, sound_config_file_path, "rb");
	if (fp)
	{
		fread(&conf, sizeof(conf), 1, fp);
		fclose(fp);
		volumeBGM_ = conf.volumeBGM;
		volumeSE_ = conf.volumeSE;
	}
	else
	{
		// メッセージボックスを使って警告表示
		MessageBox(NULL, "ファイル読込失敗", " ", MB_OK);

		conf.volumeBGM = 255;
		conf.volumeSE = 255;
		conf.version = 1.0f;

		SaveSoundConfig();
	}
}

void SoundManager::SaveSoundConfig()
{
	SoundConfigInfo conf = {};
	std::copy_n(std::begin(sound_file_signature), sizeof(char) * 4, std::begin(conf.signature));

	conf.version = sound_config_version;
	conf.volumeSE = volumeSE_;
	conf.volumeBGM = volumeBGM_;

	FILE* fp = nullptr;
	fopen_s(&fp, sound_config_file_path, "wb");
	if (fp)
	{
		fwrite(&conf, sizeof(conf), 1, fp);
		fclose(fp);
	}
}

SoundManager::~SoundManager()
{
}

void SoundManager::Play(const char* name)
{
	PlaySoundMem(nameAndHandleTable_[name], DX_PLAYTYPE_BACK);
}

void SoundManager::PlayMusic(const char* path)
{
	DxLib::PlayMusic(path, DX_PLAYTYPE_LOOP);
	SetVolumeMusic(volumeBGM_);
}

void SoundManager::SetSEVolume(int volume)
{
	for (auto& record : nameAndHandleTable_)
	{
		ChangeVolumeSoundMem(volume, record.second);
	}
	volumeSE_ = volume;
}

int SoundManager::GetSEVolume() const
{
	return volumeSE_;
}

void SoundManager::SetBGMVolume(int volume)
{
	SetVolumeMusic(volume);
	volumeBGM_ = volume;
}

int SoundManager::GetBGMVolume() const
{
	return volumeBGM_;
}

bool SoundManager::CheckMusic(const char* name)
{
	bool sound = CheckSoundMem(nameAndHandleTable_[name]);
	return sound;
}

void SoundManager::SetBGMRate(float rate)
{
	assert(0.0f <= rate && rate <= 1.0f);
	SetVolumeMusic(static_cast<int>(static_cast<float>(volumeBGM_) * rate));
}

void SoundManager::StopBGMAndSE()
{
	StopMusic();
}
