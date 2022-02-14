//=============================================================================
//
// 岩モデル処理 [rock.cpp]
// Author : 米倉睦起
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "rock.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "collision.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE			(1.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define ROCK_OFFSET_Y		(0.0f)							// 岩の足元をあわせる
#define ROCK_ATK_INTERVAL	(1.0f)							// 岩の攻撃間隔
#define ROCK_INTERVAL		(240.0f)						// 岩の生成間隔


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ROCK			g_Rock[MAX_ROCK];				// 岩

static BOOL			g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRock(void)
{
	for (int i = 0; i < MAX_ROCK; i++)
	{
		g_Rock[i].load = TRUE;

		g_Rock[i].pos = XMFLOAT3(0.0f, ROCK_OFFSET_Y, 0.0f);
		if(i == 0)
			g_Rock[i].pos = XMFLOAT3(ROCK01_POS_X, ROCK_OFFSET_Y, ROCK01_POS_Z);
		else if(i == 1)
			g_Rock[i].pos = XMFLOAT3(ROCK02_POS_X, ROCK_OFFSET_Y, ROCK02_POS_Z);
		else if(i == 2)
			g_Rock[i].pos = XMFLOAT3(ROCK03_POS_X, ROCK_OFFSET_Y, ROCK03_POS_Z);
		else if (i == 3)
			g_Rock[i].pos = XMFLOAT3(ROCK04_POS_X, ROCK_OFFSET_Y, ROCK04_POS_Z);
		else if (i == 4)
			g_Rock[i].pos = XMFLOAT3(ROCK05_POS_X, ROCK_OFFSET_Y, ROCK05_POS_Z);
		else if (i == 5)
			g_Rock[i].pos = XMFLOAT3(ROCK06_POS_X, ROCK_OFFSET_Y, ROCK06_POS_Z);

		g_Rock[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Rock[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Rock[i].size = ROCK_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Rock[0].model, &g_Rock[0].diffuse[0]);

		g_Rock[i].use = TRUE;			// 

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitRock(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].load)
		{
			UnloadModel(&g_Rock[i].model);
			g_Rock[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateRock(void)
{
	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].use != TRUE)		// この岩が使われていないなら処理をスキップ
			continue;
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawRock(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);
	// Z比較なし
	SetDepthEnable(TRUE);

	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Rock[i].scl.x, g_Rock[i].scl.y, g_Rock[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Rock[i].rot.x, g_Rock[i].rot.y + XM_PI, g_Rock[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Rock[i].pos.x, g_Rock[i].pos.y, g_Rock[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Rock[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Rock[i].model);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
	// Z比較なし
	SetDepthEnable(TRUE);

}

//=============================================================================
// 岩の取得
//=============================================================================
ROCK *GetRock(void)
{
	return &g_Rock[0];
}
