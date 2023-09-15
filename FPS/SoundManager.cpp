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
	int handle;
	std::string path = "Data/Sound/";
	path += fileName;
	path += ext;
	handle = LoadSoundMem(path.c_str());
	assert(handle >= 0);
	nameAndHandleTable_[fileName] = handle;
	return handle;
}

int SoundManager::LoadSoundFile3D(const char* fileName, const char* ext)
{
	int handle;
	std::string path = "Data/Sound/";
	path += fileName;
	path += ext;
	SetCreate3DSoundFlag(TRUE);
	handle = LoadSoundMem(path.c_str());
	SetCreate3DSoundFlag(FALSE);
	assert(handle >= 0);
	nameAndHandleTable_[fileName] = handle;
	return handle;
}

SoundManager::SoundManager()
{
	// XAudio を有効化
	SetEnableXAudioFlag(TRUE);

	// １メートルに相当する値を設定する
	Set3DSoundOneMetre(16.0f);

	LoadSoundConfig();
	SetSEVolume(volumeSE_);
	SetBGMVolume(volumeBGM_);
	LoadSoundFile3D("explosion", ".mp3");
	LoadSoundFile("gun", ".wav");
	LoadSoundFile3D("hit", ".mp3");
	LoadSoundFile("run", ".mp3");
	LoadSoundFile3D("break", ".mp3");
	LoadSoundFile3D("bee", ".mp3");
	LoadSoundFile("playerDamage", ".mp3");
	LoadSoundFile("bgm", ".mp3");
	LoadSoundFile3D("hanabi", ".mp3");
	LoadSoundFile3D("hanabi2", ".mp3");
	LoadSoundFile("gameClear", ".mp3");
	LoadSoundFile("don", ".mp3");
	LoadSoundFile("bom", ".mp3");
	LoadSoundFile("book", ".mp3");
	LoadSoundFile("GameOver", ".wav");

	SetSelectVolume("GameOver", 150);
	SetSelectVolume("break", 150);
	SetSelectVolume("explosion", 150);
	SetSelectVolume("bom", 150);
	SetSelectVolume("hit", 100);
	SetSelectVolume("gun", 100);
	SetSelectVolume("run", 150);
	SetSelectVolume("bgm", 100);	
	SetSelectVolume("gameClear", 100);
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

void SoundManager::Set3DSoundListenerPosAndFrontPos_UpVecY(VECTOR pos, VECTOR angle)
{
	DxLib::Set3DSoundListenerPosAndFrontPos_UpVecY(pos, VAdd(pos, angle));
}

SoundManager::~SoundManager()
{
}

void SoundManager::Play(const char* name)
{
	PlaySoundMem(nameAndHandleTable_[name], DX_PLAYTYPE_BACK);
}

void SoundManager::Play3D(const char* name, VECTOR soundPos, float soundRadius, bool loop)
{
	Set3DPositionSoundMem(soundPos, nameAndHandleTable_[name]);
	Set3DRadiusSoundMem(soundRadius, nameAndHandleTable_[name]);
	if (loop)
	{
		PlaySoundMem(nameAndHandleTable_[name], DX_PLAYTYPE_LOOP);
	}
	else
	{
		PlaySoundMem(nameAndHandleTable_[name], DX_PLAYTYPE_BACK);
	}
}

void SoundManager::PlayMusic(const char* name)
{
	PlaySoundMem(nameAndHandleTable_[name], DX_PLAYTYPE_LOOP);
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

void SoundManager::SetSelectVolume(const char* name, int volume)
{
	ChangeVolumeSoundMem(volume, nameAndHandleTable_[name]);
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

void SoundManager::StopSelectMusic(const char* name)
{
	StopSoundMem(nameAndHandleTable_[name]);
}

void SoundManager::StopMusic()
{
	DxLib::StopMusic();
	for (auto& sound : nameAndHandleTable_)
	{
		StopSoundMem(sound.second);
	}
}
