//=============================================================================
//
// クローンモデル処理 [clone.cpp]
// Author : 米倉睦起
//
//=============================================================================
#include "main.h"
#include "math.h"
#include "renderer.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "collision.h"
#include "player.h"
#include "coin.h"
#include "extension.h"
#include "debugproc.h"
#include "rock.h"
#include "clone.h"
#include "salieri.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	CLONE_VALUE_MOVE			(1.0f)					// 移動量
#define	CLONE_VALUE_AT_MOVE		(2.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define CLONE_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define CLONE_OFFSET_Y		(0.0f)							// エネミーの足元をあわせる
#define CLONE_EYESIGHT		(200.0f)						// エネミーの視力

#define CLONE_TPS_INTERVAL	(CLONE_POINT_INTERVAL)			//解析インターバル
#define CLONE_AT_INTERVAL	(60.0f)
#define CLONE_INVINC_FLAME	(120.0f)						// プレイヤー無敵フレーム
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CLONE			g_Clone[MAX_CLONE];				// エネミー
static BOOL				g_Load = FALSE;
static float			interval[MAX_CLONE];			//TPSを参照する間隔
static float			frameCount[MAX_CLONE];
static int				atCount;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitClone(void)
{
	for (int i = 0; i < MAX_CLONE; i++)
	{
		g_Clone[i].load = TRUE;

		g_Clone[i].pos = XMFLOAT3(0.0f, CLONE_OFFSET_Y, 0.0f);
		g_Clone[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Clone[i].nextrot = g_Clone[i].rot;
		g_Clone[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Clone[i].moveVec = { 0.0f, 0.0f, 0.0f };
		g_Clone[i].frameRot = 0.0f;
		g_Clone[i].size = CLONE_SIZE;	// 当たり判定の大きさ
		g_Clone[i].spd = 0.0f;
		g_Clone[i].life = 1;
		g_Clone[i].lifeMax = g_Clone[i].life;
		g_Clone[i].character = CLONE_OFFENSIVE;
		g_Clone[i].use = FALSE;			// TRUE:生きてる 初期化段階では未使用に

		g_Clone[i].attack = FALSE;
		g_Clone[i].attackUse = FALSE;

		g_Clone[i].atInvinc = FALSE;				//無敵時間中か否か
		g_Clone[i].atInvincTime = 0.0f;			//時間計測用
		g_Clone[i].blink = 0.0f;
		g_Clone[i].blinking = 10.0f;
		g_Clone[i].blinkingMax = 20.0f;
		g_Clone[i].blinkingCount = 2.0f;

		interval[i] = 0.0f;				//TPSを参照する間隔
		frameCount[i] = 0.0f;
	}
	atCount = 0;

	g_Load = TRUE;						//読み込み完了
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitClone(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_CLONE; i++)
	{
		if (g_Clone[i].load)
		{
			UnloadModel(&g_Clone[i].model);
			g_Clone[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateClone(void)
{
	for (int i = 0; i < MAX_CLONE; i++)
	{
		if (g_Clone[i].use != TRUE)		// このエネミーが使われていないなら処理をスキップ
			continue;

		if (g_Clone[i].life <= 0)//ライフが0なら消して次のクローン制御へ
		{
			g_Clone[i].use = FALSE;
			continue;
		}

		float old_x = g_Clone[i].pos.x;
		float old_z = g_Clone[i].pos.z;
		//インターバルカウントが一定以上か非攻撃中でなければ、TPSを元に単位ベクトルを更新
		interval[i] += 1.0f;
		if (interval[i] >= CLONE_TPS_INTERVAL && g_Clone[i].attack == FALSE)
		{
			g_Clone[i].moveVec = CloneTacticalPointSystem(i);
			interval[i] = 0.0f;
			frameCount[i] = 0.0f;
		}
		//速度を設定
		g_Clone[i].spd = CLONE_VALUE_MOVE;

		//攻撃中なら？
		if (g_Clone[i].attack == TRUE)
		{
			atCount++;
			g_Clone[i].spd = CLONE_VALUE_AT_MOVE;	//速度を上書き
			g_Clone[i].rot.x = XM_PI * 0.25f;
			if (atCount >= CLONE_AT_INTERVAL)	//攻撃フレームが終了したら攻撃をリセット
			{
				g_Clone[i].attack = FALSE;
				g_Clone[i].attackUse = FALSE;
				g_Clone[i].rot.x = 0.0f;
				atCount = 0;
			}
		}

		XMVECTOR now = XMLoadFloat3(&g_Clone[i].pos);								// 現在の場所
		XMStoreFloat3(&g_Clone[i].pos, now + g_Clone[i].moveVec * g_Clone[i].spd);	//単位ベクトルを元に移動

		if (frameCount[i] < TPS_ROT_CLONE_SPEED)
		{
			g_Clone[i].rot.y += g_Clone[i].frameRot;
			frameCount[i] += 1.0f;
		}
		else if (frameCount[i] >= TPS_ROT_CLONE_SPEED)
		{
			g_Clone[i].rot.y = g_Clone[i].nextrot.y;
		}


		for (int i = 0; i < MAX_ROCK; i++)
		{
			ROCK *rock = GetRock();
			XMFLOAT3 pos = g_Clone[i].pos;
			pos.y = 0.0f;
			if (CollisionBC(pos, rock[i].pos, g_Clone[i].size, rock[i].size) == TRUE)
			{
				g_Clone[i].pos.x = old_x;
				g_Clone[i].pos.z = old_z;
				break;
			}
		}

		//メッシュフィールド範囲外に出ないようにする
		if (g_Clone[i].pos.x <= CLONE_MIN_X ||
			g_Clone[i].pos.x >= CLONE_MAX_X ||
			g_Clone[i].pos.z <= CLONE_MIN_Z ||
			g_Clone[i].pos.z >= CLONE_MAX_Z)
		{
			g_Clone[i].pos.x = old_x;
			g_Clone[i].pos.z = old_z;
		}

		// レイキャストして足元の高さを求める
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
		XMFLOAT3 hitPosition;								// 交点
		hitPosition.y = g_Clone[i].pos.y - CLONE_OFFSET_Y;	// 外れた時用に初期化しておく
		bool ans = RayHitField(g_Clone[i].pos, &hitPosition, &normal);
		g_Clone[i].pos.y = hitPosition.y + CLONE_OFFSET_Y;


		if (g_Clone[i].atInvinc == TRUE)	//被ダメージによる無敵中にすることは？
		{
			CloneIncibleEffect(i);
			g_Clone[i].atInvincTime += 1.0f;
			if (g_Clone[i].atInvincTime >= CLONE_INVINC_FLAME)//無敵時間を終了させる
			{
				g_Clone[i].blinkingCount = 1.0f;
				g_Clone[i].blink = 0.0f;
				g_Clone[i].atInvincTime = 0.0f;
				g_Clone[i].atInvinc = FALSE;
			}
		}

	}
#ifdef _DEBUG	// デバッグ情報を表示する
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawClone(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_CLONE; i++)
	{
		if (g_Clone[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Clone[i].scl.x, g_Clone[i].scl.y, g_Clone[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Clone[i].rot.x, g_Clone[i].rot.y, g_Clone[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Clone[i].pos.x, g_Clone[i].pos.y, g_Clone[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Clone[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Clone[i].model);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
CLONE *GetClone(void)
{
	return &g_Clone[0];
}


void SetClone(XMFLOAT3 pos , int character)
{
	// 生存判定
	int clone_count = 0;
	for (int i = 0; i < MAX_CLONE; i++)
	{
		if (g_Clone[i].use == FALSE)
			continue;

		clone_count++;
	}
	if (clone_count >= MAX_CLONE)//最大数いるなら何もしない
		return;

	for (int i = 0; i < MAX_CLONE; i++)	//未使用の配列番号にアクセス
	{
		if (g_Clone[i].use != FALSE)	//使用済みならスキップ
			continue;

		g_Clone[i].pos = { pos.x, pos.y, pos.z };
		g_Clone[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Clone[i].nextrot = g_Clone[i].rot;
		g_Clone[i].scl = { 0.64f, 0.8f, 0.8f };
		g_Clone[i].use = TRUE;
		g_Clone[i].moveVec = { 0.0f, 0.0f, 0.0f };
		g_Clone[i].frameRot = 0.0f;
		g_Clone[i].size = CLONE_SIZE;			// 当たり判定の大きさ
		g_Clone[i].spd = 0.0f;
		g_Clone[i].life = 1;
		g_Clone[i].lifeMax = g_Clone[i].life;
		g_Clone[i].character = character;

		g_Clone[i].attack = FALSE;
		g_Clone[i].attackUse = FALSE;

		g_Clone[i].atInvinc = FALSE;			//無敵時間中か否か
		g_Clone[i].atInvincTime = 0.0f;			//時間計測用
		g_Clone[i].blink = 0.0f;
		g_Clone[i].blinking = 10.0f;
		g_Clone[i].blinkingMax = 20.0f;
		g_Clone[i].blinkingCount = 2.0f;

		CloneChangeDiffuse(i);
		switch(g_Clone[i].character)//キャラクター別にステータス決定
		{
		case CLONE_OFFENSIVE:
			g_Clone[i].life = 2;
			g_Clone[i].lifeMax = g_Clone[i].life;
			break;
		case CLONE_ITEM:
			g_Clone[i].life = 4;
			g_Clone[i].lifeMax = g_Clone[i].life;
			break;
		case CLONE_ALL_TAKE:
			g_Clone[i].life = 7;
			g_Clone[i].lifeMax = g_Clone[i].life;
			break;
		}
		interval[i] = 0.0f;						//TPSを参照する間隔
		frameCount[i] = 0.0f;
		break;
	}
}



void CloneChangeDiffuse(int i)
{
	//性格ごとに色を上書き
	if (g_Clone[i].character == CLONE_OFFENSIVE)
	{
		SetModelDiffuse(&g_Clone[i].model, 0, { 1.0f, 0.0f, 0.0f ,1.0f });
	}
	else if (g_Clone[i].character == CLONE_ITEM)
	{
		SetModelDiffuse(&g_Clone[i].model, 0, { 0.5f, 0.5f, 0.5f ,1.0f });
	}
	else if (g_Clone[i].character == CLONE_ALL_TAKE)
	{
		SetModelDiffuse(&g_Clone[i].model, 0, { 1.0f, 1.0f, 0.0f ,1.0f });
	}

}

void CloneIncibleEffect(int i)
{
	if (g_Clone[i].blinking < 0)
	{
		g_Clone[i].blinkingCount *= -1;
		g_Clone[i].blink -= 1.0f;
	}

	if (g_Clone[i].blinking > g_Clone[i].blinkingMax)
	{
		g_Clone[i].blinkingCount *= -1;
		g_Clone[i].blink += 1.0f;
	}
	g_Clone[i].blinking += g_Clone[i].blinkingCount;
}
