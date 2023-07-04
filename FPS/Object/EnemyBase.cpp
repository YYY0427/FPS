#include "EnemyBase.h"
#include "Player.h"
#include "../Model.h"
#include <cassert>

EnemyBase::EnemyBase()
{
}

EnemyBase::~EnemyBase()
{

}

void EnemyBase::Init()
{
	// 3Dモデルの生成
	pModel_ = std::make_shared<Model>(fileName_);
	pModel_->SetAnimation(animNo_, true, true);
	pModel_->SetUseCollision(true, true);

	// 敵をランダムに配置
	pos_.x = static_cast<float>(GetRand(2000) - 1000);
	pos_.y = 0.0f;
	pos_.z = static_cast<float>(GetRand(2000) - 1000);
	angle_ = static_cast<float>(GetRand(360) * DX_PI_F / 180.0f);

	pModel_->Update();
}

void EnemyBase::Draw()
{
	// ダメージ処理
	if (damageFrame_ > 0)
	{
		if (damageFrame_ % 2) return;
	}

	// モデルの描画
	pModel_->Draw();
}

void EnemyBase::DrawUI()
{
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	VECTOR startPos = ConvScreenPosToWorldPos(VGet(mouseX, mouseY, 0.0f));
	VECTOR endPos = ConvScreenPosToWorldPos(VGet(mouseX, mouseY, 1.0f));

	// モデルのハンドル取得
	int handle = pModel_->GetModelHandle();

	MV1SetupCollInfo(handle, -1, 8, 8, 8);

	MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(handle, -1, startPos, endPos);
	if (hitPoly.HitFlag)
	{
		// モデル内にあるHPバーを表示する座標のデータを取得する
		int frameNo = MV1SearchFrame(handle, "Head3_end");

		// HPバーを表示する座標データのワールド座標を取得する
		VECTOR hpPos = MV1GetFramePosition(handle, frameNo);

		hpPos.y += 30;

		// HPバー表示位置のワールド座標をスクリーン座標に変換する
		VECTOR screenPos = ConvWorldPosToScreenPos(hpPos);

		if ((screenPos.z <= 0.0f) || (screenPos.z >= 1.0f))
		{
			// 表示範囲外の場合何も表示しない
			return;
		}

		// 最大HPに対する現在のHPの割合を計算する
		float hpRate = static_cast<float>(hp_) / static_cast<float>(maxHp_);

		// HPバーの長さを計算する
		float barWidth = hpBarWidth_ * hpRate;

		// HPバーの土台(赤)
	//	DrawBoxAA(screenPos.x - hp_bar_width / 2, screenPos.y, screenPos.x + hp_bar_width / 2, screenPos.y + hp_bar_height, 0xff0000, true);

		// 現在のHP(緑)
		DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x - hpBarWidth_ / 2 + barWidth, screenPos.y + hpBarHeight_, 0x00ff00, true);

		// 枠線
		DrawBoxAA(screenPos.x - hpBarWidth_ / 2, screenPos.y, screenPos.x + hpBarWidth_ / 2, screenPos.y + hpBarHeight_, 0xffffff, false);

#ifdef _DEBUG
		//	DrawSphere3D(pos_, col_radius, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), false);
#endif
	}
}

int EnemyBase::GetModelHandle() const
{
	return pModel_->GetModelHandle();
}

int EnemyBase::GetColFrameIndex() const
{
	return pModel_->GetColFrameIndex();
}

bool EnemyBase::IsPlayerFront() const
{
	// 現在敵が向いている方向のベクトルを生成する
	MATRIX enemyRotMtx = MGetRotY(angle_);
	VECTOR dir = VTransform(dir_, enemyRotMtx);

	// 敵からプレイヤーへのベクトル
	VECTOR toPlayer = VSub(pPlayer_->GetPos(), pos_);
	VECTOR toPlayerNorm = VNorm(toPlayer);

	// 内積から角度を求める
	float dot = VDot(dir, toPlayerNorm);
	float angle = acosf(dot);

	// 敵からプレイヤーへのベクトル
	float distans = VSize(toPlayer);

	// 視野の中にプレイヤーがいてプレイヤーまでの距離が指定の距離以内の場合
	// プレイヤーを追いかける
	if (angle < viewAngle_ && distans < 1000.0f)
	{
		return true;
	}
	return false;
}

void EnemyBase::UpdateDead()
{
	frameCount_++;
	assert(animNo_ == deadAnimNo_);
	pModel_->Update();

	if (pModel_->IsAnimEnd() && frameCount_ > deadDisappearTime_)
	{
		isDead_ = true;
		frameCount_ = 0;
	}
}