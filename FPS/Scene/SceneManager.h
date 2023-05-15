#pragma once
#include <deque>

class Scene;
class InputState;

/// <summary>
/// シーン監視クラス
/// </summary>
class SceneManager
{
public:
	/// <summary>
	/// シーンの切り替えを行う
	/// </summary>
	/// <param name="scene">切り替えたい次シーンのアドレス</param>
	void ChangeScene(Scene* scene);

	/// <summary>
	/// シーンを上に積む
	/// Updateで実行されるのは上につまれたシーンのみ
	/// </summary>
	/// <param name="scene"></param>
	void PushScene(Scene* scene);

	/// <summary>
	/// 一番上のシーンを削除する
	/// ただし、その結果スタックが空になることのないように
	/// </summary>
	void PopScene();

	/// <summary>
	/// かっくシーンのUpdateを行う
	/// </summary>
	/// <param name="input">入力ステート</param>
	void Update(const InputState& input);
	/// <summary>
	/// 各シーンの描画を行う
	/// </summary>
	void Draw();

private:
	// 今実行中のシーンを切り替えていきたいので	
	// 参照ではなくポインタとして宣言
	std::deque<Scene*> scene_;
};

