#include "Collision.h"
#include "Object/FieldManager.h"
#include "Object/Field.h"

Collision::Collision()
{
}

Collision::~Collision()
{
}

void Collision::Init()
{
}

VECTOR Collision::ColisionToField(int modelHandle, bool isMove, bool isJump, VECTOR pos, VECTOR vec)
{
	for (auto& field : pFieldManager_->GetField())
	{
		MV1_COLL_RESULT_POLY* poly;
		HITRESULT_LINE lineRes;

		// 初期化
		bool hitFlag = false;
		int yukaNum = 0;
		int kabeNum = 0;

		// モデルの最大頂点座標と最小頂点座標の取得
		MV1_REF_POLYGONLIST refPoly = MV1GetReferenceMesh(modelHandle, -1, true);

		// 移動前のプレイヤーの座標
		VECTOR oldPos = pos;

		// 移動後のプレイヤーの座標
		moveAfterPos_ = VAdd(oldPos, vec);

		// プレイヤーとフィールドの当たり判定(何枚のポリゴンと当たっているか)
		MV1_COLL_RESULT_POLY_DIM hitDim = MV1CollCheck_Sphere(field->GetModelHandle(), -1, oldPos, (refPoly.MaxPosition.y + 50.0f) + VSize(vec));

		// 検出されたポリゴンの数だけ繰り返し
		for (int i = 0; i < hitDim.HitNum; i++)
		{
			// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
			if (hitDim.Dim[i].Normal.y < 0.000001f && hitDim.Dim[i].Normal.y > -0.000001f)
			{
				// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
				if (hitDim.Dim[i].Position[0].y > oldPos.y + 1.0f ||
					hitDim.Dim[i].Position[1].y > oldPos.y + 1.0f ||
					hitDim.Dim[i].Position[2].y > oldPos.y + 1.0f)
				{
					// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
					if (kabeNum < 2048)
					{
						// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
						kabe_[kabeNum] = &hitDim.Dim[i];

						// 壁ポリゴンの数を加算する
						kabeNum++;
					}
				}
			}
			else
			{
				// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
				if (yukaNum < 2048)
				{
					// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
					yuka_[yukaNum] = &hitDim.Dim[i];

					// 床ポリゴンの数を加算する
					yukaNum++;
				}
			}
		}

		// 壁ポリゴンとの当たり判定処理
		if (kabeNum != 0)
		{
			// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
			hitFlag = false;

			int i, j = 0;

			// 移動したかどうかで処理を分岐
			if (isMove)
			{
				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < kabeNum; i++)
				{
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					poly = kabe_[i];

					// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == false) continue;

					// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
					hitFlag = true;

					// 壁に当たったら壁に遮られない移動成分分だけ移動する
					{
						VECTOR SlideVec;	// プレイヤーをスライドさせるベクトル

						// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
						SlideVec = VCross(vec, poly->Normal);

						// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
						// 元の移動成分から壁方向の移動成分を抜いたベクトル
						SlideVec = VCross(poly->Normal, SlideVec);

						// それを移動前の座標に足したものを新たな座標とする
						moveAfterPos_ = VAdd(oldPos, SlideVec);
					}

					// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
					for (j = 0; j < kabeNum; j++)
					{
						// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
						poly = kabe_[j];

						// 当たっていたらループから抜ける
						if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
					}

					// j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
					// 壁に当たったフラグを倒した上でループから抜ける
					if (j == kabeNum)
					{
						hitFlag = false;
						break;
					}
				}
			}
			else
			{
				// 移動していない場合の処理

				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < kabeNum; i++)
				{
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					poly = kabe_[i];

					// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
					if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE)
					{
						hitFlag = true;
						break;
					}
				}
			}

			// 壁に当たっていたら壁から押し出す処理を行う
			if (hitFlag)
			{
				int k, i, j;

				// 壁からの押し出し処理を試みる最大数だけ繰り返し
				for (k = 0; k < 16; k++)
				{
					// 壁ポリゴンの数だけ繰り返し
					for (i = 0; i < kabeNum; i++)
					{
						// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
						poly = kabe_[i];

						// プレイヤーと当たっているかを判定
						if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == FALSE) continue;

						// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
						moveAfterPos_ = VAdd(moveAfterPos_, VScale(poly->Normal, 5.0f));

						// 移動した上で壁ポリゴンと接触しているかどうかを判定
						for (j = 0; j < kabeNum; j++)
						{
							// 当たっていたらループを抜ける
							poly = kabe_[j];
							if (HitCheck_Capsule_Triangle(moveAfterPos_, VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), refPoly.MaxPosition.x, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
						}

						// 全てのポリゴンと当たっていなかったらここでループ終了
						if (j == kabeNum) break;
					}

					// i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
					if (i != kabeNum) break;
				}
			}
		}
		// 床ポリゴンとの当たり判定
		if (yukaNum != 0)
		{
			// 床ポリゴンに当たったかどうかのフラグを倒しておく
			hitFlag = false;

			// 一番高い床ポリゴンにぶつける為の判定用変数を初期化
			maxY_ = 0.0f;

			// 床ポリゴンの数だけ繰り返し
			for (int i = 0; i < yukaNum; i++)
			{
				// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				poly = yuka_[i];

				// ジャンプ中かどうかで処理を分岐
				if (isJump)
				{
					// ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
					lineRes = HitCheck_Line_Triangle(VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), VAdd(moveAfterPos_, VGet(0.0f, -1.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}
				else
				{
					// 走っている場合は頭の先からそこそこ低い位置の間で当たっているかを判定( 傾斜で落下状態に移行してしまわない為 )
					lineRes = HitCheck_Line_Triangle(VAdd(moveAfterPos_, VGet(0.0f, refPoly.MaxPosition.y, 0.0f)), VAdd(moveAfterPos_, VGet(0.0f, -40.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}

				// 当たっていなかったら何もしない
				if (!lineRes.HitFlag) continue;

				// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
				if (hitFlag && maxY_ > lineRes.Position.y) continue;

				// ポリゴンに当たったフラグを立てる
				hitFlag = true;

				// 接触したＹ座標を保存する
				maxY_ = lineRes.Position.y;
			}

			// 床ポリゴンに当たったかどうかで処理を分岐
			if (hitFlag)
			{
				//// 当たった場合 ////

				if (!isJump)
				{
					// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
					moveAfterPos_.y = maxY_;
				}
			}
			else
			{
				//// 当たっていない場合 ////
				if (!isJump)
				{
					moveAfterPos_.y -= 20;
				}
			}
		}
		else
		{
			// 1枚もフィールドポリゴンと当たっていない場合落下
			moveAfterPos_.y -= 20;
		}

		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);
	}
	return moveAfterPos_;
}
