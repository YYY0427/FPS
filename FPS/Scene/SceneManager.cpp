#include "SceneManager.h"
#include "Scene.h"

void SceneManager::ChangeScene(Scene* scene)
{
	//シーンスタックが空ではなかったら...
	if (!scene_.empty())
	{
		delete scene_.front();		// topはてっぺんの要素を返してる
		scene_.pop_front();			// 1個減る -> ふつうは0になる
	}
	scene_.push_front(scene);		// 1個増える
}

void SceneManager::PushScene(Scene* scene)
{
	scene_.push_front(scene);		// 1個増える
}

void SceneManager::PopScene()
{
	//ポップの結果、シーンが0にならないようにする
	if (scene_.size() > 1)
	{
		delete scene_.front();
		scene_.pop_front();
	}
}

void SceneManager::Update(const InputState& input)
{
	scene_.front()->Update(input);
}

void SceneManager::Draw()
{
	for (int i = static_cast<int>(scene_.size() - 1); i >= 0; --i)
	{
		scene_[i]->Draw();
	}
}
