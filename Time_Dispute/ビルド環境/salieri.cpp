//=============================================================================
//
// 味方AI処理 [salieri.cpp]
// Author : 米倉睦起
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "salieri.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "collision.h"
#include "player.h"
#include "clone.h"
#include "rock.h"
#include "coin.h"
#include "extension.h"
#include "time.h"
#include "score.h"
#include "debugproc.h"
#include "clone.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define EYE_VALUE		(3.0f)				//視点の乗算評価値
#define BACK_VALUE		(0.5f)				//背面の乗算評価値
//ここからAIに使用する定義
#define CLONE_EYE_ANGLE		(XM_PI * 0.25f)	//1方向の角度
#define CLONE_VISION	(0.25f)				//視界
//TCP用の定数命名
#define AI_WAYAREA_SIZE	(5)					//ウェイポイント生成用定数
#define WAYAREA_SIZE	((AI_WAYAREA_SIZE * 2) - 1)	//ウェイポイント生成エリアの一辺のポイント数(グリッド状の正方形)
#define NEAR_CLONE		(30.0f)				//ウェイポイントに近いとはどれくらいの距離を指すのか定義
#define VALUE_VAR		(3)					//評価対象の数
#define EYE_VALUE_TPS	(1.2f)				//視点の評価係数
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CLONE_WAYPOINT	g_Clone_Waypoint[WAYAREA_SIZE][WAYAREA_SIZE];// ウェイポイント
static BOOL				g_Load = FALSE;

static float			valueCoe[VALUE_VAR];					//評価係数の保存先
//=============================================================================
// 初期化処理(全ての初期化処理の後に実行させること)
//=============================================================================
HRESULT InitSalieri(void)
{
	for (int i = 0; i < WAYAREA_SIZE; i++)
	{
		for (int k = 0; k < WAYAREA_SIZE; k++)
		{
			ZeroMemory(&g_Clone_Waypoint[i][k].material, sizeof(g_Clone_Waypoint[i][k].material));
			g_Clone_Waypoint[i][k].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
			g_Clone_Waypoint[i][k].pos = { 0.0f, 0.0f, 0.0f };			//座標
			g_Clone_Waypoint[i][k].rot = { 0.0f, 0.0f, 0.0f };			// 角度
			g_Clone_Waypoint[i][k].scl = { 1.0f, 1.0f, 1.0f };			// スケール
			g_Clone_Waypoint[i][k].value = 1.0f;			//評価値
			g_Clone_Waypoint[i][k].use = FALSE;			//使用の可否(TRUEで使用可)
		}
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSalieri(void)
{
	if (g_Load == FALSE) return;


	g_Load = FALSE;
}

//========================================================================================================================
// 対象を目視出来ているのか(引数1:自分の位置,引数2:対象の位置,引数3:自分の角度,引数4:どれくらいの距離なら目視できるのか)
//========================================================================================================================
BOOL CloneVisibility(XMFLOAT3 pos1, XMFLOAT3 pos2, float rot, float dist)
{
	BOOL ans = FALSE;

	float len = (dist) * (dist);		// 半径を2乗した物
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2点間の距離（2乗した物）
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);
	// 半径を2乗した物より距離が長い
	if (len <= lenSq)
	{
		return ans;	// 目視外にあるためこの時点で計算終了
	}

	//対象との角度を算出
	float angleX = pos2.x - pos1.x;
	float angleZ = pos2.z - pos1.z;
	float angle = atan2f(angleX, angleZ);
	//自身の角度を8方向で算出
	float stVision = 0.0f;
	float edVision = 0.0f;
	//angleがXM_PI～-XMPIなのでそれに合わせる
	//視界が値の切れ目に挟まると少し面倒な処理が必要
	if ((rot > XM_PI * (1.0f - CLONE_VISION)) || (rot < XM_PI * (CLONE_VISION - 1.0f)))
	{
		if (rot > XM_PI * (1.0f - CLONE_VISION))
		{
			stVision = rot - XM_PI * CLONE_VISION;
			edVision = (rot + XM_PI * CLONE_VISION) - (XM_PI * 2.0f);
		}
		else if (rot < XM_PI * (CLONE_VISION - 1.0f))
		{
			stVision = rot - XM_PI * CLONE_VISION + (XM_PI * 2.0f);
			edVision = rot + XM_PI * CLONE_VISION;
		}

		if (angle >= stVision || angle <= edVision)
		{
			ans = TRUE;
		}
	}
	else
	{
		stVision = rot - XM_PI * CLONE_VISION;
		edVision = rot + XM_PI * CLONE_VISION;
		if (angle >= stVision && angle <= edVision)
		{
			ans = TRUE;
		}
	}

	return ans;
}



XMVECTOR CloneTacticalPointSystem(int i)	//戦術位置解析システム。配列番号を持ってくること
{
	CLONE *clone = GetClone();
	PLAYER *player = GetPlayer();
	ROCK *rock = GetRock();
	int stWayArea = 1 - AI_WAYAREA_SIZE;
	int edWayArea = AI_WAYAREA_SIZE;
	//左下から右上にかけてウェイポイントを決定。初期化
	for (int h = stWayArea; h < edWayArea; h++)
	{
		for (int k = stWayArea; k < edWayArea; k++)
		{
			int a = h + (-stWayArea);
			int b = k + (-stWayArea);
			float x = clone[i].pos.x + CLONE_POINT_INTERVAL * k;
			float y = 0.0f;
			float z = clone[i].pos.z + CLONE_POINT_INTERVAL * h;
			g_Clone_Waypoint[a][b].pos = { x, y, z };
			//座標決定と共にフィルタリングフェーズへ
			g_Clone_Waypoint[a][b].use = FilterCloneWaypoint(g_Clone_Waypoint[a][b].pos, clone[i].pos, i);
			g_Clone_Waypoint[a][b].value = 1.0f;

			//ウェイポイント評価フェーズ
			if (g_Clone_Waypoint[a][b].use == TRUE)
				g_Clone_Waypoint[a][b].value = ValueCloneWaypoint(g_Clone_Waypoint[a][b].pos, 0.0f, i);

		}
	}

	//残ったウェイポイントの評価値を比較
	float valueMax = 0.0f;		//最大評価値を保存
	int value_x = 0;			//最大評価値の配列番号を保存
	int value_z = 0;			//
	for (int a = 0; a < WAYAREA_SIZE; a++)
	{
		for (int b = 0; b < WAYAREA_SIZE; b++)
		{
			if (g_Clone_Waypoint[a][b].use == FALSE)continue;	//FALSEの場合、削除したウェイポイントなので処理をスキップ

			if (valueMax < g_Clone_Waypoint[a][b].value)
			{
				valueMax = g_Clone_Waypoint[a][b].value;
				value_z = a;
				value_x = b;
			}
		}
	}
	//g_Clone_Waypoint[value_z][value_x].material.Diffuse = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMVECTOR vec = { 0.0f, 0.0f, 0.0f };
	if (valueMax == 0.0f) //どこにも移動候補が無い場合はそのまま返す
		return vec;

	//移動量
	float vecX = g_Clone_Waypoint[value_z][value_x].pos.x - clone[i].pos.x;
	float vecZ = g_Clone_Waypoint[value_z][value_x].pos.z - clone[i].pos.z;
	//X軸とZ軸次元での移動ベクトルを計算
	g_Clone_Waypoint[value_z][value_x].pos.y = 0.0f;
	clone[i].pos.y = 0.0f;
	vec = XMLoadFloat3(&g_Clone_Waypoint[value_z][value_x].pos) - XMLoadFloat3(&clone[i].pos);
	//単位ベクトル化。これにオブジェクトのスピードを乗算して移動させる
	vec = XMVector3Normalize(vec);
	clone[i].moveVec = vec;

	//現座標とウェイポイントの間に敵がいるなら攻撃
	CloneAttack(g_Clone_Waypoint[value_z][value_x].pos, i);

	//角度を算出し変更
	float angle = atan2f(vecX, vecZ);
	//回転量を度数に変換。こっちの方が分かりやすいため
	float degrees = XMConvertToDegrees(angle) - XMConvertToDegrees(clone[i].rot.y);
	if (degrees > 180.0f)
		degrees -= 360.0f;
	else if (degrees < -180.0f)
		degrees += 360.0f;

	clone[i].frameRot = XMConvertToRadians(degrees) / TPS_ROT_CLONE_SPEED;
	clone[i].nextrot.y = angle;
	return vec;
}

//ウェイポイントをフィルタリングする関数。ポイントの座標と自身の配列番号も持ってくる
BOOL FilterCloneWaypoint(XMFLOAT3 pos, XMFLOAT3 pos2, int k)		//ウェイポイント、自身の現座標、自身の配列番号
{
	BOOL ans = TRUE;							//まずはTRUEをセット。ここから何にも引っかからなければTRUEが返り、ウェイポイントとして使用する
	CLONE *clone = GetClone();
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	ROCK *rock = GetRock();
	//侵入不可の場所を削除
	//メッシュフィールド範囲外に出ないようにする
	if (pos.x <= CLONE_MIN_X ||
		pos.x >= CLONE_MAX_X ||
		pos.z <= CLONE_MIN_Z ||
		pos.z >= CLONE_MAX_Z)
	{
		ans = FALSE;
		return ans;
	}

	for (int i = 0; i < MAX_ROCK; i++)
	{
		//自身の座標とウェイポイントの線分が岩に当たる場合、通行不可の場所を通る必要がある為、FALSEを返す
		if (GetCrossPoints(pos2, pos, rock[i].pos, ROCK_SIZE + clone[k].size + 1.0f) == TRUE)
		{
			ans = FALSE;
			return ans;
		}
	}

	//アイテム優先のキャラは敵の近くのポイントを削除
	if (clone[k].character == CLONE_ITEM)
	{
		//他エネミーが近い場合は削除
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			if (CollisionBC(enemy[i].pos, pos, clone[i].size, NEAR_CLONE) == TRUE)
			{
				ans = FALSE;						//他エネミーが近いためFALSEを返す
				return ans;
			}
		}
	}

	return ans;
}

//ウェイポイントを評価する関数。自身の配列番号も持ってくる
//引数1:ウェイポイント座標、引数2:最初にある評価値、引数3:配列番号
float ValueCloneWaypoint(XMFLOAT3 pos, float value, int k)
{
	COIN *coin = GetCoin();
	EXTEN *exten = GetExten();
	PLAYER *player = GetPlayer();
	CLONE *clone = GetClone();
	ENEMY *enemy = GetEnemy();
	ValueCloneCoefficient(k);
	for (int i = 0; i < MAX_COIN; i++)
	{
		if (valueCoe[CLONE_ITEM_COIN] == 0.0f || coin[i].use != TRUE)		//評価係数が0もしくはフィールドに無いなら計算不要
			continue;
		XMVECTOR disP = XMLoadFloat3(&coin[i].pos) - XMLoadFloat3(&pos);
		disP = XMVector3Length(disP);		//距離を2乗
		float point;
		XMStoreFloat(&point, disP);
		if (point < 1.0f)			//座標差分が1.0以下なら評価最大値を出力
		{
			value += valueCoe[CLONE_ITEM_COIN];
		}
		else
		{
			point = 1.0f / point;					//離れているほど係数を小さくなるようにする。
			value += valueCoe[CLONE_ITEM_COIN] * point;	//評価係数と距離の評価を乗算
		}
	}

	for (int i = 0; i < MAX_EXTEN; i++)
	{
		if (valueCoe[CLONE_ITEM_EXTEN] == 0.0f || exten[i].use != TRUE)		//評価係数が0もしくはフィールドに無いなら計算不要
			continue;
		XMVECTOR disP = XMLoadFloat3(&exten[i].pos) - XMLoadFloat3(&pos);
		disP = XMVector3Length(disP);			//距離を2乗
		float point;
		XMStoreFloat(&point, disP);
		if (point < 1.0f)			//座標差分が1.0以下なら評価最大値を出力
		{
			value += valueCoe[CLONE_ITEM_EXTEN];
		}
		else
		{
			point = 1.0f / point;			//離れているほど係数を小さくなるようにする。
			value += valueCoe[CLONE_ITEM_EXTEN] * point;	//評価係数と距離の評価を乗算
		}
	}

	if (valueCoe[CLONE_ENEMY] == 0.0f)			//評価係数が0なら計算不要。このまま返す
		return value;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE)continue;

		XMVECTOR disP = XMLoadFloat3(&enemy[i].pos) - XMLoadFloat3(&pos);
		disP = XMVector3Length(disP);				//距離を2乗
		float point;
		XMStoreFloat(&point, disP);
		if (point < 1.0f)				//座標差分が1.0以下なら評価最大値を出力
		{
			value += valueCoe[CLONE_ENEMY];
		}
		else
		{
			point = 1.0f / point;						//離れているほど係数を小さくなるようにする。
			value += valueCoe[CLONE_ENEMY] * point;		//評価係数と距離の評価を乗算
		}
	}
	//視界内の評価値を乗算
	if (CloneVisibility(clone[k].pos, pos, clone[k].rot.y, CLONE_POINT_INTERVAL * CLONE_POINT_INTERVAL) == TRUE)
	{
		value *= EYE_VALUE_TPS;
	}

	return value;
}

//直接評価法による評価の重みづけとそれに基づくオブジェクト毎の評価係数を算出。
//(それぞれのオブジェクトに対する点数の合計) / (評価したオブジェクト数)
//どういった性格にするのかを求めるため配列番号を持ってくる
void ValueCloneCoefficient(int k)
{
	CLONE *clone = GetClone();
	float value = 1.0f;			//(総和係数)
	float coinValue = 0.0f;		//コインに対する初期の重み
	float extenValue = 0.0f;	//時間延長アイテムに対する初期の重み
	float playerValue = 0.0f;	//脅威対象に対する初期の重み
	float weight[VALUE_VAR] = { 0.0f, 0.0f, 0.0f };		//重み
	float sumWeight = 0.0f;
	//状況分析をし、それぞれの評価に重みをつける

	//性格ごとに評価に重みづけ
	if (clone[k].character == CLONE_OFFENSIVE)	//攻撃的
	{
		weight[CLONE_ENEMY] += 4.0f;
		weight[CLONE_ITEM_COIN] += 1.0f;
	}
	else if (clone[k].character == CLONE_ITEM)	//時間アイテムの優先
	{
		weight[CLONE_ITEM_COIN] += 1.0f;
		weight[CLONE_ITEM_EXTEN] += 4.0f;
	}
	else if (clone[k].character == CLONE_ALL_TAKE)		//スコアアイテムの優先
	{
		weight[CLONE_ITEM_COIN] += 4.0f;
		weight[CLONE_ITEM_EXTEN] += 1.0f;
		weight[CLONE_ENEMY] += 1.0f;
	}


	sumWeight = weight[CLONE_ITEM_COIN] + weight[CLONE_ITEM_EXTEN] + weight[CLONE_ENEMY];
	valueCoe[CLONE_ITEM_COIN] = weight[CLONE_ITEM_COIN] / sumWeight;		//コインに対する重み
	valueCoe[CLONE_ITEM_EXTEN] = weight[CLONE_ITEM_EXTEN] / sumWeight;		//時間延長アイテムに対する重み
	valueCoe[CLONE_ENEMY] = weight[CLONE_ENEMY] / sumWeight;	//プレイヤーに対する重み
}

//戦術的優位箇所か判定
//引数1:ウェイポイント座標、引数2:敵の座標、引数3:配列番号
float CloneTacticalSuperiority(XMFLOAT3 pos1, XMFLOAT3 pos2,int k)
{
	float ans = 1.0f;	//まずは1.0で初期化。係数として使用するため、初期値は1.0
	ROCK *rock = GetRock();
	//障害物とウェイポイントの座標の間に敵がいる→ハイドポイント発見。評価係数を高くする
	for (int i = 0; i < MAX_ROCK; i++)
	{
		if ((GetCrossPoints(pos1, pos2, rock[i].pos, ROCK_SIZE) == TRUE))
			ans += 0.1f;
	}

	return ans;
}
/////////////////////////////////////////////////////////////////////
//選択されたウェイポイント上にエネミーがいるなら攻撃するようにする。引数1:ウェイポイント座標、引数2:クローンの配列番号
/////////////////////////////////////////////////////////////////////
void CloneAttack(XMFLOAT3 pos, int k)
{
	ENEMY *enemy = GetEnemy();
	CLONE *clone = GetClone();
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE)continue;	//死んでるエネミーは探査不要

		if (GetCrossPoints(pos, clone[k].pos, enemy[i].pos, enemy[i].size + clone[k].size) != TRUE)
			continue;						//当たってないなら次のエネミーへ
			
		//ここまで来たら当たってるため、攻撃コマンドをオンにし、探査終了
		clone[k].attack = TRUE;
		clone[k].attackUse = TRUE;
		return;
	}
}