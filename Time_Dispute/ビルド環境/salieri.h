//=============================================================================
//
// 味方AI処理 [salieri.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	CLONE_MIN_VALUE		(0.0f)					// 最低評価値
#define	CLONE_MAX_VALUE		(100.0f)				// 最高評価値
#define TPS_CLONE_SPEED		(1.0f)					//エネミーの移動速度(戦術位置解析AIで使用)
#define TPS_ROT_CLONE_SPEED (10.0f)					//何フレームでエネミーの回転を終わらせるか
#define CLONE_POINT_INTERVAL (20.0f)			//ウェイポイント生成間隔

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct CLONE_WAYPOINT
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
	CLONE_ITEM_COIN,
	CLONE_ITEM_EXTEN,
	CLONE_ENEMY,
	CLONE_NON
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSalieri(void);
void UninitSalieri(void);
BOOL CloneVisibility(XMFLOAT3 pos1, XMFLOAT3 pos2, float rot, float dist);
XMVECTOR CloneTacticalPointSystem(int i);
BOOL FilterCloneWaypoint(XMFLOAT3 pos, XMFLOAT3 pos2, int k);
float ValueCloneWaypoint(XMFLOAT3 pos, float value, int k);
void ValueCloneCoefficient(int k);
float CloneTacticalSuperiority(XMFLOAT3 pos1, XMFLOAT3 pos2, int k);
void CloneAttack(XMFLOAT3 pos, int k);