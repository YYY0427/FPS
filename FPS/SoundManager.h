#pragma once
#include <unordered_map>
#include <string>
#include <DxLib.h>

/// <summary>
/// SEの管理を行うシングルトンクラス
/// </summary>
class SoundManager
{
private:
	// 変更したサウンド情報をファイルに書き込む
	struct SoundConfigInfo
	{
		char signature[5];			// "SND_"
		float version;				// 1.0
		unsigned short volumeSE;	// 0～255	
		unsigned short volumeBGM;	// 0～255
	};

	int volumeSE_ = 255;
	int volumeBGM_ = 255;

	std::unordered_map<std::string, int> nameAndHandleTable_;
	int LoadSoundFile(const char* fileName, const char* ext);
	int LoadSoundFile3D(const char* fileName, const char* ext);

	// シングルトンパターンなのでコンストラクタはprivateに置く
	SoundManager();

	// コピーも代入も禁止する
	SoundManager(const SoundManager&) = delete;			// コピーコンストラクタ
	void operator = (const SoundManager&) = delete;		// 代入も禁止

	void LoadSoundConfig();
public:
	~SoundManager();
	/// <summary>
	/// SoundManager使用者はGetInstance()を通した参照からしか利用できない
	/// </summary>
	/// <returns></returns>
	static SoundManager& GetInstance()
	{
		// 唯一の実態
		static SoundManager instance;

		// それの参照を返す
		return instance;
	}

	/// <summary>
	/// 指定のサウンドを鳴らす
	/// </summary>
	/// <param name="name">サウンド名</param>
	void Play(const char* name);
	void Play3D(const char* name, VECTOR soundPos, float soundRadius, bool loop);
	void PlayMusic(const char* name);

	/// <summary>
	/// SEのボリュームを設定する
	/// </summary>
	/// <param name="volume"></param>
	void SetSEVolume(int volume);
	int GetSEVolume() const;

	/// <summary>
	/// BGMのボリュームを設定する
	/// </summary>
	/// <param name="volume"></param>
	void SetBGMVolume(int volume);
	int GetBGMVolume() const;

	void SetSelectVolume(const char* name, int volume);

	/// <summary>
	/// BGMの現在の鳴らす音量の割合
	/// </summary>
	/// <param name="rate">音量の割合(一時的なもの0.0～1.0)</param>
	void SetBGMRate(float rate);

	/// <summary>
	/// 指定のサウンドが再生中かチェック
	/// </summary>
	/// <param name="name">サウンド名</param>
	/// <returns> true 再生中  false 再生されていない</returns>
	bool CheckMusic(const char* name);

	// ストップしたいサウンドを選んでストップ
	void StopSelectMusic(const char* name);

	// サウンドを全部一時停止する
	void StopMusic();

	// サウンド情報をセーブ
	void SaveSoundConfig();

	// リスナーの位置を向きを設定
	void Set3DSoundListenerPosAndFrontPos_UpVecY(VECTOR pos, VECTOR anglee);
};