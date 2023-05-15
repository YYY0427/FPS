#pragma once

//プロトタイプ宣言
class SceneManager;	//シーンマネージャー
class InputState;	//入力ステート

//この時点ではSceneManagerとInputStateは
//ポインタもしくは参照としてしか扱えない。

/// <summary>
/// シーン基底クラス
/// </summary>
class Scene
{
protected:
	// シーンマネーシャーへの参照
	SceneManager& manager_; 
public:
	Scene(SceneManager& manager) :
		manager_(manager)
	{
	}
	virtual ~Scene() {};

	// シーンを更新する
	virtual void Update(const InputState& input) = 0;

	// シーンを描画する
	virtual void Draw() = 0;
};

