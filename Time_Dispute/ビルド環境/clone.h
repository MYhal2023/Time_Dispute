//=============================================================================
//
// クローンモデル処理 [enemy.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_CLONE			"data/MODEL/Slime.obj"			// 読み込むモデル名
#define MAX_CLONE		(5)									// エネミーの数
#define CLONE_PARTS_NUM		(1)								// 1エネミーのパーツの数
#define CLONE_PARTS_MAX		(MAX_CLONE * CLONE_PARTS_NUM)	// エネミーの必要総パーツ数

#define	CLONE_SIZE		(10.0f)								// 当たり判定の大きさ
#define	CLONE_BATLE_SIZE (150.0f)							// プレイヤー探査判定の大きさ
#define CLONE_DEFER				(2.0f)							// エネミー移動制限の余剰幅
#define CLONE_MIN_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + CLONE_DEFER * BLOCK_SIZE)				// エネミー移動上限
#define CLONE_MIN_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - (FIELD_Z - CLONE_DEFER) * BLOCK_SIZE)	// 
#define CLONE_MAX_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + (FIELD_X - CLONE_DEFER) * BLOCK_SIZE )	// 
#define CLONE_MAX_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - CLONE_DEFER * BLOCK_SIZE)				// 
#define CLONE_OFFENSIVE_VALUE (40)
#define CLONE_TAKE_TIME_VALUE (80)
#define CLONE_CROW_VALUE (100)

//クローンの性格
enum
{
	CLONE_OFFENSIVE,	//攻撃的
	CLONE_ITEM,			//アイテムのみ
	CLONE_ALL_TAKE,		//全て取得
	CLONE_MAX			//最大値(参照しない)
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct CLONE
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			nextrot;			// モデルの次の向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)
	XMVECTOR			moveVec;			//モデルの動く方向
	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	//AIから来る移動情報
	float				frameRot;			//1フレームごとの回転量

	float				size;				// 当たり判定の大きさ
	float				spd;
	//ライフ
	int					life;
	int					lifeMax;

	//AI
	int					character;			//性格
	//攻撃
	BOOL				attack;			//攻撃中か否か
	BOOL				attackUse;		//この攻撃判定が使用されたか

	BOOL				atInvinc;				//無敵時間中か否か
	float				atInvincTime;			//時間計測用
	float				blink;
	float				blinking;
	float				blinkingMax;
	float				blinkingCount;

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitClone(void);
void UninitClone(void);
void UpdateClone(void);
void DrawClone(void);

CLONE *GetClone(void);
void SetClone(XMFLOAT3 pos, int character);
void CloneChangeDiffuse(int i);
void CloneIncibleEffect(int i);