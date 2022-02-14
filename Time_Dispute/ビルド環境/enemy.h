//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/Eden.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_PARTS	"data/MODEL/Eden_origin.obj"	// 読み込むモデル名
#define MAX_ENEMY		(12)									// エネミーの数
#define ENEMY_PARTS_NUM		(1)								// 1エネミーのパーツの数
#define ENEMY_PARTS_MAX		(MAX_ENEMY * ENEMY_PARTS_NUM)	// エネミーの必要総パーツ数

#define	ENEMY_SIZE		(10.0f)								// 当たり判定の大きさ
#define	ENEMY_BATLE_SIZE (150.0f)							// プレイヤー探査判定の大きさ
#define DEFER				(2.0f)							// エネミー移動制限の余剰幅
#define ENEMY_MIN_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + DEFER * BLOCK_SIZE)				// エネミー移動上限
#define ENEMY_MIN_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - (FIELD_Z - DEFER) * BLOCK_SIZE)	// 
#define ENEMY_MAX_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + (FIELD_X - DEFER) * BLOCK_SIZE )	// 
#define ENEMY_MAX_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - DEFER * BLOCK_SIZE)				// 

//エネミーの性格
enum
{
	PLANE,		//プレーン
	OFFENSIVE,	//攻撃的
	TAKE_TIME,	//時間奪取
	CROW,		//スコアアイテム優先
	MAX			//最大値(参照しない)
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
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
	int					shadowIdx;			// 影のインデックス番号
	
	INTERPOLATION_DATA	*tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間

	//ライフ
	int					life;
	int					lifeMax;

	//AI
	float				atInterval;			//エネミーの攻撃間隔
	BOOL				atBullet;
	int					character;			//性格
	float				playerValue;		//プレイヤー評価値
	float				extenValue;			//時間優先評価値
	float				scoreValue;			//スコア優先評価値

	int					status;				//現ステート
	float				motion;				//ステート遷移
	// 親は、NULL、子供は親のアドレスを入れる
	ENEMY				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
ENEMY *GetParts(void);
void SetEnemy(void);
void ChangeDiffuse(int i);
void AttackBullet(int i);