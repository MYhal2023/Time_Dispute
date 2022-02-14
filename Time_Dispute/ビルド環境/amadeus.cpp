//=============================================================================
//
// エネミーAI処理 [amadeus.cpp]
// Author : 米倉睦起
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "amadeus.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "collision.h"
#include "player.h"
#include "enemy.h"
#include "rock.h"
#include "coin.h"
#include "extension.h"
#include "time.h"
#include "score.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define EYE_VALUE		(3.0f)				//視点の乗算評価値
#define BACK_VALUE		(0.5f)				//背面の乗算評価値
//ここからAIに使用する定義
#define EYE_ANGLE		(XM_PI * 0.25f)		//1方向の角度
#define ENEMY_VISION	(0.25f)				//エネミーの視界
#define ANGLE_TYPE		(4)					//方角の数
#define AI_ROCK_SIZE	(4)					//岩の侵入不可のサイズ
//TCP用の定数命名
#define AI_WAYAREA_SIZE	(5)					//ウェイポイント生成用定数
#define WAYAREA_SIZE	((AI_WAYAREA_SIZE * 2) - 1)	//ウェイポイント生成エリアの一辺のポイント数(グリッド状の正方形)
#define NEAR_ENEMY		(40.0f)				//ウェイポイントに近いとはどれくらいの距離を指すのか定義
#define VALUE_VAR		(3)					//評価対象の数
#define EYE_VALUE_TPS	(1.2f)				//視点の評価係数
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static WAYPOINT			g_WayPoint[WAYAREA_SIZE][WAYAREA_SIZE];// ウェイポイント

static BOOL				g_Load = FALSE;

static float			valueCoe[VALUE_VAR];					//評価係数の保存先
//=============================================================================
// 初期化処理(全ての初期化処理の後に実行させること)
//=============================================================================
HRESULT InitAmadeus(void)
{
	for (int i = 0; i < WAYAREA_SIZE; i++)
	{
		for (int k = 0; k < WAYAREA_SIZE; k++)
		{
			ZeroMemory(&g_WayPoint[i][k].material, sizeof(g_WayPoint[i][k].material));
			g_WayPoint[i][k].pos = { 0.0f, 0.0f, 0.0f };			//座標
			g_WayPoint[i][k].rot = { 0.0f, 0.0f, 0.0f };			// 角度
			g_WayPoint[i][k].scl = { 1.0f, 1.0f, 1.0f };			// スケール
			g_WayPoint[i][k].value = 1.0f;			//評価値
			g_WayPoint[i][k].use = FALSE;			//使用の可否(TRUEで使用可)
		}
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitAmadeus(void)
{
	if (g_Load == FALSE) return;


	g_Load = FALSE;
}

//========================================================================================================================
// 対象を目視出来ているのか(引数1:自分の位置,引数2:対象の位置,引数3:自分の角度,引数4:どれくらいの距離なら目視できるのか)
//========================================================================================================================
BOOL Visibility(XMFLOAT3 pos1, XMFLOAT3 pos2, float rot, float dist)
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
	if ((rot > XM_PI * (1.0f - ENEMY_VISION)) || (rot < XM_PI * (ENEMY_VISION - 1.0f)))
	{
		if (rot > XM_PI * (1.0f - ENEMY_VISION))
		{
			stVision = rot - XM_PI * ENEMY_VISION;
			edVision = (rot + XM_PI * ENEMY_VISION) - (XM_PI * 2.0f);
		}
		else if (rot < XM_PI * (ENEMY_VISION - 1.0f))
		{
			stVision = rot - XM_PI * ENEMY_VISION + (XM_PI * 2.0f);
			edVision = rot + XM_PI * ENEMY_VISION;
		}

		if (angle >= stVision || angle <= edVision)
		{
			ans = TRUE;
		}
	}
	else
	{
		stVision = rot - XM_PI * ENEMY_VISION;
		edVision = rot + XM_PI * ENEMY_VISION;
		if (angle >= stVision && angle <= edVision)
		{
			ans = TRUE;
		}
	}

	return ans;
}

XMVECTOR TacticalPointSystem(int i)	//戦術位置解析システム。配列番号を持ってくること
{
	ENEMY *enemy = GetEnemy();
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
			float x = enemy[i].pos.x + POINT_INTERVAL * k;
			float y = 0.0f;
			float z = enemy[i].pos.z + POINT_INTERVAL * h;
			g_WayPoint[a][b].pos = { x, y, z };
			//座標決定と共にフィルタリングフェーズへ
			g_WayPoint[a][b].use = FilterWayPoint(g_WayPoint[a][b].pos, enemy[i].pos, i);
			g_WayPoint[a][b].value = 1.0f;


			//ウェイポイント評価フェーズ
			if (g_WayPoint[a][b].use == TRUE)
				g_WayPoint[a][b].value = ValueWayPoint(g_WayPoint[a][b].pos, 0.0f, i);

			////デバッグ時、分かりやすいように色覚を変える
			//if (g_WayPoint[a][b].use == TRUE)
			//	g_WayPoint[a][b].material.Diffuse = { 0.0f + g_WayPoint[a][b].value, 0.0f, 0.0f, 1.0f };
			//else
			//	g_WayPoint[a][b].material.Diffuse = { 0.0f, 0.0f, 1.0f, 1.0f };
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
			if (g_WayPoint[a][b].use == FALSE)continue;	//FALSEの場合、削除したウェイポイントなので処理をスキップ

			if (valueMax < g_WayPoint[a][b].value)
			{
				valueMax = g_WayPoint[a][b].value;
				value_z = a;
				value_x = b;
			}
		}
	}
	//g_WayPoint[value_z][value_x].material.Diffuse = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMVECTOR vec = { 0.0f, 0.0f, 0.0f };
	if (valueMax == 0.0f) //どこにも移動候補が無い場合はそのまま返す
		return vec;

	//移動量
	float vecX = g_WayPoint[value_z][value_x].pos.x - enemy[i].pos.x;
	float vecZ = g_WayPoint[value_z][value_x].pos.z - enemy[i].pos.z;
	//X軸とZ軸次元での移動ベクトルを計算
	g_WayPoint[value_z][value_x].pos.y = 0.0f;
	enemy[i].pos.y = 0.0f;
	vec = XMLoadFloat3(&g_WayPoint[value_z][value_x].pos) - XMLoadFloat3(&enemy[i].pos);
	//単位ベクトル化。これにエネミーのスピードをかける
	vec = XMVector3Normalize(vec);
	enemy[i].moveVec = vec;

	//角度を算出し変更
	float angle = atan2f(vecX, vecZ);
	//回転量を度数に変換。こっちの方が分かりやすいため
	float degrees = XMConvertToDegrees(angle) - XMConvertToDegrees(enemy[i].rot.y);
	if (degrees > 180.0f)
		degrees -= 360.0f;
	else if (degrees < -180.0f)
		degrees += 360.0f;
	enemy[i].frameRot = XMConvertToRadians(degrees) / TPS_ROT_ENEMY_SPEED;
	enemy[i].nextrot.y = angle;
	return vec;
}

//ウェイポイントをフィルタリングする関数。ポイントの座標と自身の配列番号も持ってくる
BOOL FilterWayPoint(XMFLOAT3 pos, XMFLOAT3 pos2, int k)		//ウェイポイント、自身の現座標、自身の配列番号
{
	BOOL ans = TRUE;							//まずはTRUEをセット。ここから何にも引っかからなければTRUEが返り、ウェイポイントとして使用する
	ENEMY *enemy = GetEnemy();
	PLAYER *player = GetPlayer();
	ROCK *rock = GetRock();
	XMFLOAT3 checkPos = {player->pos.x, 0.0f, player->pos.z};
	//侵入不可の場所を削除
	//メッシュフィールド範囲外に出ないようにする
	if (pos.x <= ENEMY_MIN_X ||
		pos.x >= ENEMY_MAX_X ||
		pos.z <= ENEMY_MIN_Z ||
		pos.z >= ENEMY_MAX_Z)
	{
		ans = FALSE;
		return ans;
	}

	for (int i = 0; i < MAX_ROCK; i++)
	{
		//自身の座標とウェイポイントの線分が岩に当たる場合、通行不可の場所を通る必要がある為、FALSEを返す
		if (GetCrossPoints(pos2, pos, rock[i].pos, ROCK_SIZE + enemy[k].size) == TRUE)
		{
			ans = FALSE;
			return ans;
		}
	}

	////他エネミーが近い場合は削除
	//for (int i = 0; i < MAX_ENEMY; i++)
	//{
	//	if (i == k)continue;					//自身の近くはフィルタリングしない

	//	if (CollisionBC(enemy[i].pos, pos, enemy[i].size, NEAR_ENEMY) == TRUE)
	//	{
	//		ans = FALSE;						//他エネミーが近いためFALSEを返す
	//		return ans;
	//	}
	//}

	//性格が逃げならば、プレイヤーに近い場所を削除。プレイヤーから離れる挙動に
	if ((CollisionBC(player->pos, pos, player->size, NEAR_ENEMY) == TRUE ||
		(GetCrossPoints(pos2, pos, player->pos, player->size) == TRUE)) &&
		(enemy[k].character == PLANE))
	{
		ans = FALSE;						//プレイヤーが近いためFALSEを返す
		return ans;
	}

	return ans;
}

//ウェイポイントを評価する関数。自身の配列番号も持ってくる
//引数1:ウェイポイント座標、引数2:最初にある評価値、引数3:配列番号
float ValueWayPoint(XMFLOAT3 pos, float value, int k)
{
	COIN *coin = GetCoin();
	EXTEN *exten = GetExten();
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	ValueCoefficient(k);
	for (int i = 0; i < MAX_COIN; i++)
	{
		if (valueCoe[ITEM_COIN] == 0.0f || coin[i].use != TRUE)		//評価係数が0もしくはフィールドに無いなら計算不要
			continue;
		XMVECTOR disP = XMLoadFloat3(&coin[i].pos) - XMLoadFloat3(&pos);
		disP = XMVector3Length(disP);		//距離を2乗
		float point;
		XMStoreFloat(&point, disP);
		if (point < 1.0f)			//座標差分が1.0以下なら評価最大値を出力
		{
			value += valueCoe[ITEM_COIN];
		}
		else
		{
			point = 1.0f / point;					//離れているほど係数を小さくなるようにする。
			value += valueCoe[ITEM_COIN] * point;	//評価係数と距離の評価を乗算
		}
	}

	for (int i = 0; i < MAX_EXTEN; i++)
	{
		if (valueCoe[ITEM_EXTEN] == 0.0f || exten[i].use != TRUE)		//評価係数が0もしくはフィールドに無いなら計算不要
			continue;
		XMVECTOR disP = XMLoadFloat3(&exten[i].pos) - XMLoadFloat3(&pos);
		disP = XMVector3Length(disP);			//距離を2乗
		float point;
		XMStoreFloat(&point, disP);
		if (point < 1.0f)			//座標差分が1.0以下なら評価最大値を出力
		{
			value += valueCoe[ITEM_EXTEN];
		}
		else
		{
			point = 1.0f / point;			//離れているほど係数を小さくなるようにする。
			value += valueCoe[ITEM_EXTEN] * point;	//評価係数と距離の評価を乗算
		}
	}

	if (valueCoe[MAIN_PLAYER] == 0.0f)			//評価係数が0なら計算不要。このまま返す
		return value;
	XMVECTOR disP = XMLoadFloat3(&player->pos) - XMLoadFloat3(&pos);
	disP = XMVector3Length(disP);				//距離を2乗
	float point;
	XMStoreFloat(&point, disP);
	if (point < 1.0f)				//座標差分が1.0以下なら評価最大値を出力
	{
		value += valueCoe[MAIN_PLAYER];
	}
	else
	{
		point = 1.0f / point;						//離れているほど係数を小さくなるようにする。
		value += valueCoe[MAIN_PLAYER] * point;		//評価係数と距離の評価を乗算
	}

	if (Visibility(enemy[k].pos, pos, enemy[k].rot.y, POINT_INTERVAL * POINT_INTERVAL) == TRUE)
	{
		value *= EYE_VALUE_TPS;
	}
	//戦術的優位さに応じて評価を高くする
	value *= TacticalSuperiority(pos, player->pos, k);

	return value;
}

//直接評価法による評価の重みづけとそれに基づくオブジェクト毎の評価係数を算出。
//(それぞれのオブジェクトに対する点数の合計) / (評価したオブジェクト数)
//どういった性格にするのかを求めるため配列番号を持ってくる
void ValueCoefficient(int k)
{
	ENEMY *enemy = GetEnemy();
	float value = 1.0f;			//(総和係数)
	float coinValue = 0.0f;		//コインに対する初期の重み
	float extenValue = 0.0f;	//時間延長アイテムに対する初期の重み
	float playerValue = 0.0f;	//脅威対象に対する初期の重み
	float weight[VALUE_VAR] = { 0.0f, 0.0f, 0.0f };		//重み
	float sumWeight = 0.0f;
	//状況分析をし、それぞれの評価に重みをつける
	if (GetTime() >= 30)
	{
		weight[ITEM_EXTEN] += 2.0f;
	}

	//エネミーの性格ごとに評価に重みづけ
	if (enemy[k].character == PLANE)			//逃げる？
	{
		weight[ITEM_COIN] += 1.0f;
		weight[ITEM_EXTEN] += 1.0f;
	}
	else if (enemy[k].character == OFFENSIVE)	//攻撃的
	{
		weight[MAIN_PLAYER] += 2.0f;
	}
	else if (enemy[k].character == TAKE_TIME)	//時間アイテムの優先
	{
		weight[ITEM_COIN] += 1.0f;
		weight[ITEM_EXTEN] += 3.0f;
		weight[MAIN_PLAYER] += 1.0f;
	}
	else if (enemy[k].character == CROW)		//スコアアイテムの優先
	{
		weight[ITEM_COIN] += 3.0f;
		weight[ITEM_EXTEN] += 1.0f;
		weight[MAIN_PLAYER] += 1.0f;
	}

	////デバッグ用の重み付け！プレイヤー視点からポイントを見たい時に使う！
	//weight[ITEM_COIN] += 1.0f;
	//weight[ITEM_EXTEN] += 1.0f;
	//weight[MAIN_PLAYER] += 1.0f;
	//ここまで！

	sumWeight = weight[ITEM_COIN] + weight[ITEM_EXTEN] + weight[MAIN_PLAYER];
	valueCoe[ITEM_COIN] = weight[ITEM_COIN] / sumWeight;		//コインに対する重み
	valueCoe[ITEM_EXTEN] = weight[ITEM_EXTEN] / sumWeight;		//時間延長アイテムに対する重み
	valueCoe[MAIN_PLAYER] = weight[MAIN_PLAYER] / sumWeight;	//プレイヤーに対する重み
}

//戦術的優位箇所か判定
//引数1:ウェイポイント座標、引数2:敵の座標、引数3:配列番号
float TacticalSuperiority(XMFLOAT3 pos1, XMFLOAT3 pos2,int k)
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