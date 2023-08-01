#pragma once
#include "Scene.h"
#include <memory>
#include <vector>
#include <DxLib.h>

class Camera;
class Player;
class Shot;
class EnemyManager;
class SkyDoom;
class Tower;
class Collision;
class StageManager;
class EnemyShotFactory;
class ObstacleManager;
class BomManager;
class UI;

/// <summary>
/// メインシーン
/// <summary>
class MainScene : public Scene
{
public:
	MainScene(SceneManager& manager, StageManager* pStageManager);
	virtual ~MainScene();

	void Update(const InputState& input);
	void Draw();

	// ショットを撃つ
	void StartPlayerShot(VECTOR pos, VECTOR vec);

	// レティクルの位置の取得
	float GetReticlePosX() const;
	float GetReticlePosY() const;

	// プレイヤーがステージから落ちたときのフェード
	void PlayerFallFade();
private:
	// フェードアウト
	void FadeOutUpdate(const InputState& input);

	// フェードイン
	void FadeInUpdate();

	// 通常のアップデート
	void NormalUpdate(const InputState& input);

private:
	// メンバ関数ポインタ
	using UpdateFunc_t = void (MainScene::*) (const InputState& input);
	UpdateFunc_t updateFunc_;

	// ポインタ
	std::shared_ptr<Camera> pCamera_ ;
	std::shared_ptr<Player> pPlayer_;
	std::shared_ptr<EnemyManager> pEnemyManager_;
	std::vector<std::shared_ptr<Shot>> pShot_;
	std::shared_ptr<BomManager> pBomManager_;
	std::shared_ptr<SkyDoom> pSkyDoom_;
	std::shared_ptr<Tower> pTower_;
	std::shared_ptr<Collision> pCollision_;
	std::shared_ptr<EnemyShotFactory> pEnemyShotFactory_;
	std::shared_ptr<ObstacleManager> pObstacleManager_;
	std::shared_ptr<UI> pUI_;
	StageManager* pStageManager_;	 // シーンがなくなっても消えてほしくないのでshard_ptrを使わない

	// 画像ハンドル
	int gameOverUIhandle_;
	int gameClearUIhandle_;
	int towerIconHandle_;
	int playerIconHandle_;
	int gunUIhandle_;
	int bomUIhandle_;
	int infinityHandle_;
	int completeHandle_;
	int playerDamageUIHandle_;

	// フォントハンドル
	int bulletCounFontHandle_;
	int towerNameFontHandle_;
	int towerNameShadowFontHandle_;
	int playerNameFontHandle_;
	int playerNameShadowFontHandle_;
	int gameClearFontHandle_;

	// フェイド
	int fadeTimer_;
	int fadeValue_;
	int gameOverUIfadeValue_;
	int gameOverUIFadeTimer_;
	int gameClearUIFadeValue_;
	int gameClearUIFadeTimer_;
	int playerDamageUIFadeTimer_;
	int playerDamageUIFadeValue_;

	// シャドウマップ
	int shadowMap_;

	// ゲームオーバーかどうか
	bool isGameOver_;

	// プレイヤーショットと敵に弾が当たったか
	bool isHit_;

	// 敵に攻撃を当てたときに表示するレティクルのエフェクトの表示の持続時間
	int reticleEffectDisplayTime;

	// ゲームクリア画像の拡大率
	double gameClearImgExRate_;

	// ゲームクリアしてからの経過時間
	int gameClearCount_;

	// プレイヤーがダメージを受けたか
	bool isPlayerDamage_;

	bool isPass_;
};