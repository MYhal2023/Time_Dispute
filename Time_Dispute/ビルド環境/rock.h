//=============================================================================
//
// 岩モデル処理 [rock.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ROCK			"data/MODEL/iwa.obj"			// 読み込むモデル名
#define MAX_ROCK		(6)					// 岩の数

#define	ROCK_SIZE		(30.0f)				// 当たり判定の大きさの半径
#define	ROCK01_X_AI		(20)				// AIに引き渡す岩の座標情報
#define	ROCK01_Z_AI		(30)				// 
#define ROCK01_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK01_X_AI * BLOCK_SIZE)
#define ROCK01_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK01_Z_AI * BLOCK_SIZE)

#define	ROCK02_X_AI		(80)				// AIに引き渡す岩の座標情報
#define	ROCK02_Z_AI		(60)				// 
#define ROCK02_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK02_X_AI * BLOCK_SIZE)
#define ROCK02_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK02_Z_AI * BLOCK_SIZE)

#define	ROCK03_X_AI		(100)				// AIに引き渡す岩の座標情報
#define	ROCK03_Z_AI		(90)				// 
#define ROCK03_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK03_X_AI * BLOCK_SIZE)
#define ROCK03_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK03_Z_AI * BLOCK_SIZE)

#define	ROCK04_X_AI		(30)				// AIに引き渡す岩の座標情報
#define	ROCK04_Z_AI		(80)				// 
#define ROCK04_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK04_X_AI * BLOCK_SIZE)
#define ROCK04_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK04_Z_AI * BLOCK_SIZE)

#define	ROCK05_X_AI		(50)				// AIに引き渡す岩の座標情報
#define	ROCK05_Z_AI		(20)				// 
#define ROCK05_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK05_X_AI * BLOCK_SIZE)
#define ROCK05_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK05_Z_AI * BLOCK_SIZE)

#define	ROCK06_X_AI		(90)				// AIに引き渡す岩の座標情報
#define	ROCK06_Z_AI		(30)				// 
#define ROCK06_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK06_X_AI * BLOCK_SIZE)
#define ROCK06_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK06_Z_AI * BLOCK_SIZE)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ROCK
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				size;				// 当たり判定の大きさ
	
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRock(void);
void UninitRock(void);
void UpdateRock(void);
void DrawRock(void);

ROCK *GetRock(void);
BOOL CheckHitBattle(int i);