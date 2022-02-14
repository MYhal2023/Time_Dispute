//=============================================================================
//
// エネミーAI処理 [amadeus.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MIN_VALUE			(0.0f)					// 最低評価値
#define	MAX_VALUE			(100.0f)				// 最高評価値
#define TPS_ENEMY_SPEED		(1.0f)					//エネミーの移動速度(戦術位置解析AIで使用)
#define TPS_ROT_ENEMY_SPEED (10.0f)					//何フレームでエネミーの回転を終わらせるか
#define POINT_INTERVAL		(20.0f)					//ウェイポイント生成間隔

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct WAYPOINT
{
	XMFLOAT4X4			mtxWorld;		// ワールドマトリックス
	MATERIAL			material;		// マテリアル
	XMFLOAT3			pos;			//座標
	XMFLOAT3			rot;			// 角度
	XMFLOAT3			scl;			// スケール
	float				value;			//評価値
	BOOL				use;			//使用の可否(trueで使用可)
};
//認識させるオブジェクトを列挙
enum
{
	ITEM_COIN,
	ITEM_EXTEN,
	MAIN_PLAYER,
	NON
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitAmadeus(void);
void UninitAmadeus(void);
BOOL Visibility(XMFLOAT3 pos1, XMFLOAT3 pos2, float rot, float dist);
XMVECTOR TacticalPointSystem(int i);
BOOL FilterWayPoint(XMFLOAT3 pos, XMFLOAT3 pos2, int k);
float ValueWayPoint(XMFLOAT3 pos, float value, int k);
void ValueCoefficient(int k);
float TacticalSuperiority(XMFLOAT3 pos1, XMFLOAT3 pos2, int k);