#pragma once
#include <unordered_map>
#include <string>
#include <DxLib.h>

/// <summary>
/// SE�̊Ǘ����s���V���O���g���N���X
/// </summary>
class SoundManager
{
private:
	// �ύX�����T�E���h�����t�@�C���ɏ�������
	struct SoundConfigInfo
	{
		char signature[5];			// "SND_"
		float version;				// 1.0
		unsigned short volumeSE;	// 0�`255	
		unsigned short volumeBGM;	// 0�`255
	};

	int volumeSE_ = 255;
	int volumeBGM_ = 255;

	std::unordered_map<std::string, int> nameAndHandleTable_;
	int LoadSoundFile(const char* fileName, const char* ext);
	int LoadSoundFile3D(const char* fileName, const char* ext);

	// �V���O���g���p�^�[���Ȃ̂ŃR���X�g���N�^��private�ɒu��
	SoundManager();

	// �R�s�[��������֎~����
	SoundManager(const SoundManager&) = delete;			// �R�s�[�R���X�g���N�^
	void operator = (const SoundManager&) = delete;		// ������֎~

	void LoadSoundConfig();
public:
	~SoundManager();
	/// <summary>
	/// SoundManager�g�p�҂�GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
	/// </summary>
	/// <returns></returns>
	static SoundManager& GetInstance()
	{
		// �B��̎���
		static SoundManager instance;

		// ����̎Q�Ƃ�Ԃ�
		return instance;
	}

	/// <summary>
	/// �w��̃T�E���h��炷
	/// </summary>
	/// <param name="name">�T�E���h��</param>
	void Play(const char* name);
	void Play3D(const char* name, VECTOR soundPos, float soundRadius, bool loop);
	void PlayMusic(const char* name);

	/// <summary>
	/// SE�̃{�����[����ݒ肷��
	/// </summary>
	/// <param name="volume"></param>
	void SetSEVolume(int volume);
	int GetSEVolume() const;

	/// <summary>
	/// BGM�̃{�����[����ݒ肷��
	/// </summary>
	/// <param name="volume"></param>
	void SetBGMVolume(int volume);
	int GetBGMVolume() const;

	void SetSelectVolume(const char* name, int volume);

	/// <summary>
	/// BGM�̌��݂̖炷���ʂ̊���
	/// </summary>
	/// <param name="rate">���ʂ̊���(�ꎞ�I�Ȃ���0.0�`1.0)</param>
	void SetBGMRate(float rate);

	/// <summary>
	/// �w��̃T�E���h���Đ������`�F�b�N
	/// </summary>
	/// <param name="name">�T�E���h��</param>
	/// <returns> true �Đ���  false �Đ�����Ă��Ȃ�</returns>
	bool CheckMusic(const char* name);

	// �X�g�b�v�������T�E���h��I��ŃX�g�b�v
	void StopSelectMusic(const char* name);

	// �T�E���h��S���ꎞ��~����
	void StopMusic();

	// �T�E���h�����Z�[�u
	void SaveSoundConfig();

	// ���X�i�[�̈ʒu��������ݒ�
	void Set3DSoundListenerPosAndFrontPos_UpVecY(VECTOR pos, VECTOR anglee);
};