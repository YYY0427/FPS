#include "Model.h"
#include <cassert>

namespace
{
	// 当たり判定として使用するフレームの名前
	const char* const collision_frame_name = "Collision";
}

Model::Model(const char* fileName) :
	m_isUseCollision(false),
	m_isUpdateColision(false),
	m_colFrameIndex(-1),
	m_animChangeFrame(0),
	m_animChangeFrameTotal(0)
{
	m_modelHandle = MV1LoadModel(fileName);

	// モデルのロードに失敗した
	assert(m_modelHandle != -1);

	// アニメーション情報のクリア
	ClearAnimData(m_animPrev);
	ClearAnimData(m_animNext);
}

Model::Model(int orgModel) :
	m_isUseCollision(false),
	m_isUpdateColision(false),
	m_colFrameIndex(-1),
	m_animChangeFrame(0),
	m_animChangeFrameTotal(0)
{
	// ロードに失敗したモデルがコピー元として指定されている
	assert(m_modelHandle != -1);
	m_modelHandle = MV1DuplicateModel(orgModel);

	// アニメーション情報のクリア
	ClearAnimData(m_animPrev);
	ClearAnimData(m_animNext);
}

Model::~Model()
{
	if (m_isUpdateColision)
	{
		// 当たり判定情報を使用していたら破棄する
		MV1TerminateCollInfo(m_modelHandle, -1);
		m_isUseCollision = false;
	}
	MV1DeleteModel(m_modelHandle);
}

void Model::SetUseCollision(bool isUse, bool isNeedUpdate)
{
	// 当たり判定を使わないのに当たり判定の更新を毎フレームおこないたい場合をはじく
	assert(isUse || !isNeedUpdate);

	// 当たり判定使用情報に変化があった場合のみ更新する
	if (m_isUpdateColision != isUse)
	{
		if (isUse)
		{
			// 使わない→使う
			// 当たり判定用フレームを検索
			m_colFrameIndex = MV1SearchFrame(m_modelHandle, collision_frame_name);

			if (m_colFrameIndex < 0)	// 見つからなかった or エラー
			{
				m_colFrameIndex = -1;
			}
			MV1SetupCollInfo(m_modelHandle, m_colFrameIndex, 8, 8, 8);
		}
		else
		{
			// 使う→使わない
			MV1TerminateCollInfo(m_modelHandle, -1);
		}
	}

	m_isUseCollision = isUse;
	m_isUpdateColision = isNeedUpdate;
}

void Model::Update()
{
	UpdateAnim(m_animPrev);
	UpdateAnim(m_animNext);

	// 指定フレームかけてアニメーションを変更する
	m_animChangeFrame++;
	if (m_animChangeFrame > m_animChangeFrameTotal)
	{
		m_animChangeFrame = m_animChangeFrameTotal;
	}

	// アニメーションのブレンド率を設定する
	UpdateAnimBlendRate();

	// 当たり判定データの更新
	if (m_isUseCollision && m_isUpdateColision)
	{
		MV1RefreshCollInfo(m_modelHandle, m_colFrameIndex);
	}
}

void Model::Draw()
{
	MV1DrawModel(m_modelHandle);
}

void Model::SetPos(VECTOR pos)
{
	MV1SetPosition(m_modelHandle, pos);
}

void Model::SetRot(VECTOR rot)
{
	MV1SetRotationXYZ(m_modelHandle, rot);
}

void Model::SetScale(VECTOR scale)
{
	MV1SetScale(m_modelHandle, scale);
}

void Model::SetAnimation(int animNo, bool isLoop, bool isForceChange)
{
	if (!isForceChange)
	{
		// すでに再生されているアニメは再生しない
		if (m_animNext.animNo == animNo)	return;
	}

	// 以前のアニメーションが残っていれば終了
	if (m_animPrev.attachNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_animPrev.attachNo);
		ClearAnimData(m_animPrev);
	}
	if (m_animNext.attachNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_animNext.attachNo);
		ClearAnimData(m_animNext);
	}
	// 新しくアニメーションを設定
	m_animNext.animNo = animNo;
	m_animNext.attachNo = MV1AttachAnim(m_modelHandle, animNo, -1, false);
	m_animNext.totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_animNext.attachNo);
	m_animNext.isLoop = isLoop;

	m_animChangeFrameTotal = 1;
	m_animChangeFrame = 1;
}

void Model::ChangeAnimation(int animNo, bool isLoop, bool isForceChange, int changeFrame)
{
	if (!isForceChange)
	{
		// すでに再生されているアニメは再生しない
		if (m_animNext.animNo == animNo)	return;
	}

	// 以前のアニメーションが残っていれば終了
	if (m_animPrev.attachNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_animPrev.attachNo);
		ClearAnimData(m_animPrev);
	}
	// 現在再生しているアニメーションを古いアニメとする
	m_animPrev = m_animNext;

	// 新しくアニメーションを設定
	m_animNext.animNo = animNo;
	m_animNext.attachNo = MV1AttachAnim(m_modelHandle, animNo, -1, false);
	m_animNext.totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_animNext.attachNo);
	m_animNext.isLoop = isLoop;

	// 変更にかけるフレーム数を覚えておく
	m_animChangeFrameTotal = changeFrame;
	m_animChangeFrame = 0;

	// アニメーションのブレンド率を設定する
	UpdateAnimBlendRate();
}

bool Model::IsAnimEnd()
{
	// Looppアニメの場合は常にfalseを返す
	if (m_animNext.isLoop) return false;

	float time = MV1GetAttachAnimTime(m_modelHandle, m_animNext.attachNo);

	if (time >= m_animNext.totalTime) return true;

	return false;
}

void Model::ClearAnimData(AnimData& anim)
{
	anim.animNo = -1;
	anim.attachNo = -1;
	anim.totalTime = 0.0f;
	anim.isLoop = false;
}

void Model::UpdateAnim(AnimData anim, float dt)
{
	// アニメーションが設定されていない場合は何もしない
	if (anim.attachNo == -1) return;

	// アニメーションの更新
	float time = MV1GetAttachAnimTime(m_modelHandle, anim.attachNo);
	time += dt;
	if (time > anim.totalTime)
	{
		if (anim.isLoop)
		{
			// アニメーションのループ
			time -= anim.totalTime;
		}
		else
		{
			// 最後の状態で止まる
			time = anim.totalTime;
		}
	}
	MV1SetAttachAnimTime(m_modelHandle, anim.attachNo, time);
}

void Model::UpdateAnimBlendRate()
{
	// アニメーション変化のフレーム数に応じたブレンド率を設定する
	float rate = static_cast<float> (m_animChangeFrame) / static_cast<float>(m_animChangeFrameTotal);
	if (rate > 1.0f)	rate = 1.0f;
	MV1SetAttachAnimBlendRate(m_modelHandle, m_animPrev.attachNo, 1.0f - rate);
	MV1SetAttachAnimBlendRate(m_modelHandle, m_animNext.attachNo, rate);
}