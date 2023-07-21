#include "Collision.h"
#include "StageManager.h"
#include "StageBase.h"
#include "Object/Tower.h"
#include "Object/EnemyManager.h"
#include "Object/EnemyBase.h"
#include "Obstacle.h"
#include "ObstacleManager.h"
#include "Object/Player.h"

Collision::Collision(StageManager* pStages, std::shared_ptr<Tower> pTower, std::shared_ptr<EnemyManager> pEnemyManager, std::shared_ptr<ObstacleManager> pObstacleManager, std::shared_ptr<Player> pPlayer) :
	moveAfterPos_(VGet(0, 0, 0)),
	oldPos_(VGet(0, 0, 0)),
	yukaNum_(0),
	kabeNum_(0),
	isHitFlag_(false),
	pStages_(pStages),
	pTower_(pTower),
	pEnemyManager_(pEnemyManager),
	pObstacleManager_(pObstacleManager),
	pPlayer_(pPlayer)
{
}

Collision::~Collision()
{
}

void Collision::Init()
{
	moveAfterPos_ = VGet(0, 0, 0);
	oldPos_ = VGet(0, 0, 0);
	yukaNum_ = 0;
	kabeNum_ = 0;
	isHitFlag_ = false;
}

void Collision::CollCheck(int characterModelHandle, int objectModelHandle, int collisionFrameIndex, VECTOR pos, VECTOR vec, float collisionRadius, int chara)
{
	// モデルの最大頂点座標と最小頂点座標の取得
	refPoly_ = MV1GetReferenceMesh(characterModelHandle, -1, true);

	// 移動前の座標
	oldPos_ = pos;

	// 移動後の座標
	moveAfterPos_ = VAdd(oldPos_, vec);

	// モデルとフィールドの当たり判定(何枚のポリゴンと当たっているか)
//	hitDim_ = MV1CollCheck_Sphere(objectModelHandle, -1, oldPos_, (refPoly_.MaxPosition.y + 50.0f) + VSize(vec));
	hitDim_ = MV1CollCheck_Capsule(objectModelHandle, collisionFrameIndex, oldPos_, VGet(oldPos_.x, oldPos_.y + refPoly_.MaxPosition.y, oldPos_.z), collisionRadius);

	// 検出されたポリゴンの数だけ繰り返し
	for (int i = 0; i < hitDim_.HitNum; i++)
	{
		// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
	//	if (hitDim_.Dim[i].Normal.y < 0.000001f && hitDim_.Dim[i].Normal.y > -0.000001f)
		if (hitDim_.Dim[i].Normal.y < 0.4f && hitDim_.Dim[i].Normal.y > -0.000001f)
		{
			// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
			if (hitDim_.Dim[i].Position[0].y > oldPos_.y + 1.0f ||
				hitDim_.Dim[i].Position[1].y > oldPos_.y + 1.0f ||
				hitDim_.Dim[i].Position[2].y > oldPos_.y + 1.0f)
			{
				// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
				if (kabeNum_ < 2048)
				{
					// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
					kabe_[kabeNum_] = &hitDim_.Dim[i];

					// 壁ポリゴンの数を加算する
					kabeNum_++;
				}
			}
		}
		else
		{
			// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
			if (yukaNum_ < 2048)
			{
				// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
				yuka_[yukaNum_] = &hitDim_.Dim[i];

				// 床ポリゴンの数を加算する
				yukaNum_++;
			}
		}
	}
}

void Collision::WallPolyColCheckProcess(bool isMove, VECTOR vec)
{
	// 壁ポリゴンとの当たり判定処理
	if (kabeNum_ != 0)
	{
		// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
		isHitFlag_ = false;

		int i, j = 0;

		// 移動したかどうかで処理を分岐
		if (isMove)
		{
			// 壁ポリゴンの数だけ繰り返し
			for (i = 0; i < kabeNum_; i++)
			{
				// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				poly_ = kabe_[i];

				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
				if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == FALSE) continue;

				// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
				isHitFlag_ = true;

				// 壁に当たったら壁に遮られない移動成分分だけ移動する
				{
					// プレイヤーをスライドさせるベクトル
					VECTOR SlideVec;	

					// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
					SlideVec = VCross(vec, poly_->Normal);

					// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
					// 元の移動成分から壁方向の移動成分を抜いたベクトル
					SlideVec = VCross(poly_->Normal, SlideVec);

					// それを移動前の座標に足したものを新たな座標とする
					moveAfterPos_ = VAdd(oldPos_, SlideVec);
				}

				// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
				for (j = 0; j < kabeNum_; j++)
				{
					// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					poly_ = kabe_[j];

					// 当たっていたらループから抜ける
					if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == TRUE) break;
				}

				// j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
				// 壁に当たったフラグを倒した上でループから抜ける
				if (j == kabeNum_)
				{
					isHitFlag_ = false;
					break;
				}
			}
		}
		else
		{
			// 移動していない場合の処理

			// 壁ポリゴンの数だけ繰り返し
			for (i = 0; i < kabeNum_; i++)
			{
				// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				poly_ = kabe_[i];

				// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
				if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == TRUE)
				{
					isHitFlag_ = true;
					break;
				}
			}
		}

		// 壁に当たっていたら壁から押し出す処理を行う
		if (isHitFlag_)
		{
			int k, i, j;

			// 壁からの押し出し処理を試みる最大数だけ繰り返し
			for (k = 0; k < 32; k++)
			{
				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < kabeNum_; i++)
				{
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					poly_ = kabe_[i];

					// プレイヤーと当たっているかを判定
					if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == FALSE) continue;

					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
				//	moveAfterPos_ = VAdd(moveAfterPos_, VScale(poly_->Normal, 5.0f));
					moveAfterPos_ = VAdd(moveAfterPos_, VScale(poly_->Normal, 0.1f));

					// 移動した上で壁ポリゴンと接触しているかどうかを判定
					for (j = 0; j < kabeNum_; j++)
					{
						// 当たっていたらループを抜ける
						poly_ = kabe_[j];
						if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), refPoly_.MaxPosition.x, poly_->Position[0], poly_->Position[1], poly_->Position[2]) == TRUE) break;
					}

					// 全てのポリゴンと当たっていなかったらここでループ終了
					if (j == kabeNum_) break;
				}

				// i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
				if (i != kabeNum_) break;
			}
		}
	}
}

void Collision::FloorPolyColCheckProcess(bool isJump, bool isUseGravity, int chara)
{
	// 床ポリゴンとの当たり判定
	if (yukaNum_ != 0)
	{
		// 床ポリゴンに当たったかどうかのフラグを倒しておく
		isHitFlag_ = false;

		// 一番高い床ポリゴンにぶつける為の判定用変数を初期化
		float minY = 0.0f;

		// 床ポリゴンの数だけ繰り返し
		for (int i = 0; i < yukaNum_; i++)
		{
			// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
			poly_ = yuka_[i];

			// ジャンプ中かどうかで処理を分岐
			if (isJump)
			{
				// ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
				lineRes_ = HitCheck_Line_Triangle(VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), VAdd(moveAfterPos_, VGet(0.0f, -1.0f, 0.0f)), poly_->Position[0], poly_->Position[1], poly_->Position[2]);
			}
			else
			{
				// 走っている場合は頭の先からそこそこ低い位置の間で当たっているかを判定( 傾斜で落下状態に移行してしまわない為 )
				lineRes_ = HitCheck_Line_Triangle(VAdd(moveAfterPos_, VGet(0.0f, refPoly_.MaxPosition.y, 0.0f)), VAdd(moveAfterPos_, VGet(0.0f, -40.0f, 0.0f)), poly_->Position[0], poly_->Position[1], poly_->Position[2]);
			}

			// 当たっていなかったら何もしない
			if (!lineRes_.HitFlag) continue;

			// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
			if (isHitFlag_ && minY > lineRes_.Position.y) continue;

			// ポリゴンに当たったフラグを立てる
			isHitFlag_ = true;

			// 接触したＹ座標を保存する
			minY = lineRes_.Position.y;
		}

		// 床ポリゴンに当たったかどうかで処理を分岐
		if (isHitFlag_)
		{
			//// 当たった場合 ////
			
			// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
			moveAfterPos_.y = minY;	

			// 接触した床ポリゴンを保存
			groundY_ = minY;
		}
		else
		{
			//// 当たっていない場合 ////

			if (isUseGravity)
			{
				// 床ポリゴンと当たっていない場合落下
				moveAfterPos_.y -= 20;
			}

			// 地面が存在しないのでありえない値を代入
			groundY_ = -10000.0f;
		}
	}
	else
	{
		//// 当たっていない場合 ////

		if (isUseGravity)
		{
			// 床ポリゴンと当たっていない場合落下
			moveAfterPos_.y -= 20;
		}

		// 地面が存在しないのでありえない値を代入
		groundY_ = -10000.0f;
	}
}

void Collision::JumpingFloorPolyColCheckProcess()
{
}

VECTOR Collision::Colision(int modelHandle, bool isMove, bool isJump, bool isUseGravity, VECTOR pos, VECTOR vec, int chara, float collisionRadius)
{
	// 初期化
	Init();

	// タワーとの当たり判定チェック
	CollCheck(modelHandle, pTower_->GetModelHandle(), -1, pos, vec, collisionRadius, chara);

	for (auto& obj : pObstacleManager_->GetObstacles())
	{
		// 障害物との当たり判定チェック
		CollCheck(modelHandle, obj->GetModelHandle(), -1, pos, vec, collisionRadius, chara);
	}

	for (auto& enemy : pEnemyManager_->GetEnemies())
	{
		if (enemy->GetModelHandle() == modelHandle) continue;

		// 敵同士の当たり判定チェック
		CollCheck(modelHandle, enemy->GetModelHandle(), enemy->GetColFrameIndex(), pos, vec, collisionRadius, chara);
	}

	// フィールドとの当たり判定チェック
	CollCheck(modelHandle, pStages_->GetStages()->GetModelHandle(), -1, pos, vec, collisionRadius, chara);

	if (tower != chara)
	{
		// 壁ポリゴン処理
		WallPolyColCheckProcess(isMove, vec);
	}

	// 床ポリゴン処理
	FloorPolyColCheckProcess(isJump, isUseGravity, chara);

	// 検出したプレイヤーの周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(hitDim_);

	return moveAfterPos_;
}