//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
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
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "collision.h"
#include "player.h"
#include "coin.h"
#include "extension.h"
#include "amadeus.h"
#include "debugproc.h"
#include "rock.h"
#include "time.h"
#include "clone.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE			(1.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define ENEMY_OFFSET_Y		(8.0f)							// エネミーの足元をあわせる
#define ENEMY_ATK_INTERVAL	(1.0f)							// エネミーの攻撃間隔
#define ENEMY_INTERVAL		(400.0f)						// エネミーの生成間隔
#define POP_ENEMY_INTERVAL	(40.0f)							// エネミーの生成間隔を減少させる
#define POP_TIME_ENEMY_INTERVAL	(15.0f)						// エネミーの生成間隔を減少させる
#define MOTION_INTERVAL		(240.0f)						// エネミーのステート遷移間隔
#define ENEMY_EYESIGHT		(200.0f)						// エネミーの視力
#define ENEMY_MEMORY		(180.0f)						// エネミーの記憶時間
#define MAX_MEMORY			(10)							// エネミーの記憶量
#define ENEMY_OFFSET		(40)							// エネミーの出現座標調整

#define TPS_INTERVAL		(POINT_INTERVAL)				//解析インターバル
#define AT_INTERVAL			(240.0f)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー
static ENEMY			g_Parts[ENEMY_PARTS_MAX];		// エネミーのパーツ
static BOOL				g_Load = FALSE;
static float			Pop;							//エネミーを出すときのインターバルカウント変数
static float			popInterval;
static float			interval[MAX_ENEMY];			//TPSを参照する間隔
static float			frameCount[MAX_ENEMY];
static INTERPOLATION_DATA move_tbl_parts[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 2, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].moveVec = { 0.0f, 0.0f, 0.0f };
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ


		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);

		g_Enemy[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Enemy[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		g_Enemy[i].life = 1;
		g_Enemy[i].lifeMax = g_Enemy[i].life;
		g_Enemy[i].atInterval = 0.0f;
		g_Enemy[i].character = PLANE;
		g_Enemy[i].motion = 0.0f;
		g_Enemy[i].use = FALSE;			// TRUE:生きてる 初期化段階では未使用に

		// パーツの初期化
		for (int k = 0; k < ENEMY_PARTS_MAX; k++)
		{
			if (g_Parts[k].use == FALSE)
			{
				// 位置・回転・スケールの初期設定
				g_Parts[k].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_Parts[k].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_Parts[k].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

				// 親子関係
				g_Parts[k].parent = &g_Enemy[i];		// ここに親のアドレスを入れる

				// 階層アニメーション用のメンバー変数の初期化
				g_Parts[k].tbl_adr = NULL;				// 再生するアニメデータの先頭アドレスをセット
				g_Parts[k].move_time = 0.0f;			// 実行時間をクリア
				g_Parts[k].tbl_size = 0;				// 再生するアニメデータのレコード数をセット
			}
		}

		for (int k = 0; k < ENEMY_PARTS_NUM; k++)		//パーツが増える事を予想して、それに対応させておく
		{
			int partsNum = i * ENEMY_PARTS_NUM + k;		//パーツの添え字を求める。現在はエネミーの添え字がそのまま来るようになっている
			g_Parts[partsNum].use = TRUE;
			g_Parts[partsNum].parent = &g_Enemy[i];		// 親をセット

			if (k == 0)
			{
				g_Parts[partsNum].tbl_adr = move_tbl_parts;		// 再生するアニメデータの先頭アドレスをセット
				g_Parts[partsNum].tbl_size = sizeof(move_tbl_parts) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
				g_Parts[partsNum].load = TRUE;
			}
		}
	}

	Pop = 0.0f;				//エネミーを出すときのインターバルカウント変数
	popInterval = 0.0f;		//エネミー生成間隔
	g_Load = TRUE;			//読み込み完了
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	// パーツの初期化
	for (int i = 0; i < ENEMY_PARTS_MAX; i++)
	{
		if (g_Parts[i].load)
		{
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}
	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use != TRUE)		// このエネミーが使われていないなら処理をスキップ
			continue;
		//インターバルカウントが一定以上あれば、TPSを元に単位ベクトルを更新
		interval[i] += 1.0f;
		if (interval[i] >= TPS_INTERVAL)
		{
			g_Enemy[i].moveVec = TacticalPointSystem(i);
			interval[i] = 0.0f;
			frameCount[i] = 0.0f;
		}
		XMVECTOR now = XMLoadFloat3(&g_Enemy[i].pos);								// 現在の場所
		XMStoreFloat3(&g_Enemy[i].pos, now + g_Enemy[i].moveVec * TPS_ENEMY_SPEED);	//単位ベクトルを元に移動

		if (frameCount[i] < TPS_ROT_ENEMY_SPEED)
		{
			g_Enemy[i].rot.y += g_Enemy[i].frameRot;
			frameCount[i] += 1.0f;
		}
		else if (frameCount[i] >= TPS_ROT_ENEMY_SPEED)
		{
			g_Enemy[i].rot.y = g_Enemy[i].nextrot.y;
		}
		AttackBullet(i);

		// レイキャストして足元の高さを求める
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
		XMFLOAT3 hitPosition;								// 交点
		hitPosition.y = g_Enemy[i].pos.y - ENEMY_OFFSET_Y;	// 外れた時用に初期化しておく
		bool ans = RayHitField(g_Enemy[i].pos, &hitPosition, &normal);
		g_Enemy[i].pos.y = hitPosition.y + ENEMY_OFFSET_Y;


		// パーツの階層アニメーション
		for (int k = 0; k < ENEMY_PARTS_NUM; k++)
		{
			int partsNum = i * ENEMY_PARTS_NUM + k;		//パーツの添え字を求める。現在はエネミーの添え字がそのまま来るようになっている
			// 使われているなら処理する
			if ((g_Parts[partsNum].use == TRUE) && (g_Parts[partsNum].tbl_adr != NULL))
			{
				// 移動処理
				int		index = (int)g_Parts[partsNum].move_time;
				float	time = g_Parts[partsNum].move_time - index;
				int		size = g_Parts[partsNum].tbl_size;

				float dt = 1.0f / g_Parts[partsNum].tbl_adr[index].frame;	// 1フレームで進める時間
				g_Parts[partsNum].move_time += dt;							// アニメーションの合計時間に足す

				if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				{
					g_Parts[partsNum].move_time = 0.0f;
					index = 0;
				}

				// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				XMVECTOR p1 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 1].pos);	// 次の場所
				XMVECTOR p0 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 0].pos);	// 現在の場所
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Parts[partsNum].pos, p0 + vec * time);

				// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				XMVECTOR r1 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 1].rot);	// 次の角度
				XMVECTOR r0 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 0].rot);	// 現在の角度
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Parts[partsNum].rot, r0 + rot * time);

				// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				XMVECTOR s1 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 1].scl);	// 次のScale
				XMVECTOR s0 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 0].scl);	// 現在のScale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Parts[partsNum].scl, s0 + scl * time);

			}
		}
	}
#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Enemy:X:%d C:%d\n", g_Enemy[0].status, g_Enemy[1].status);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Enemy[i].model);

		// パーツの反映
		for (int k = 0; k < ENEMY_PARTS_NUM; k++)	//エネミーのパーツ数分だけマトリクスを反映
		{
			int partsNum = i * ENEMY_PARTS_NUM + k;		//パーツの添え字を求める。現在はエネミーの添え字がそのまま来るようになっている
			
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[partsNum].scl.x, g_Parts[partsNum].scl.y, g_Parts[partsNum].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[partsNum].rot.x, g_Parts[partsNum].rot.y, g_Parts[partsNum].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[partsNum].pos.x, g_Parts[partsNum].pos.y, g_Parts[partsNum].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[partsNum].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[partsNum].parent->mtxWorld));
				// ↑
				// g_Player.mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Parts[partsNum].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_Parts[partsNum].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// モデル描画
			DrawModel(&g_Parts[partsNum].model);

		}

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy(void)
{
	return &g_Enemy[0];
}

ENEMY *GetParts(void)
{
	return &g_Parts[0];
}


void SetEnemy(void)
{
	// エネミーの生存判定
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE)
			continue;

		enemy_count++;
	}
	if (enemy_count >= MAX_ENEMY)//エネミーが最大数で既にいるなら何もしない
		return;
#ifdef DEBUG

	if (GetKeyboardPress(DIK_P))
		return;
#endif

	//エネミースポーン処理
	Pop += 1.0f;
	popInterval = ENEMY_INTERVAL;
	int time = GetTime();
	//時間が経つほどエネミーの生成間隔を短くする
	while (time > 0)
	{
		time -= (int)(POP_TIME_ENEMY_INTERVAL);
		popInterval -= POP_ENEMY_INTERVAL;
	}

	if (Pop < popInterval)
		return;
	//エネミー発生に伴って初期化を行う
	Pop = 0.0f;
	for (int i = 0; i < MAX_ENEMY; i++)	//未使用の配列番号にアクセス
	{
		if (g_Enemy[i].use != FALSE)	//使用済みならスキップ
			continue;

		//出現場所の設定
		XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
		//マップの端に来ないかつ障害物内でない座標間でランダムに座標を決定
		ROCK *rock = GetRock();
		BOOL ans = TRUE;
		float posX;
		float posZ;
		while (ans == FALSE)
		{
			posX = (float)(ENEMY_OFFSET + rand() % (FIELD_X - (ENEMY_OFFSET * 2 - 1)));
			posZ = (float)(ENEMY_OFFSET + rand() % (FIELD_Z - (ENEMY_OFFSET * 2 - 1)));
			pos.x = -(FIELD_X * 0.5f) * BLOCK_SIZE + (posX * BLOCK_SIZE);
			pos.z = (FIELD_Z * 0.5f) * BLOCK_SIZE - (posZ * BLOCK_SIZE);
			ans = TRUE;
			for (int k = 0; k < MAX_ROCK; k++)
			{
				if ((CollisionBC(pos, rock[k].pos, g_Enemy[i].size, rock[k].size) == TRUE))
					ans = FALSE;
			}
		}
		g_Enemy[i].pos = { pos.x, pos.y, pos.z };
		g_Enemy[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Enemy[i].nextrot = g_Enemy[i].rot;
		g_Enemy[i].scl = { 1.0f, 1.0f, 1.0f };
		g_Enemy[i].use = TRUE;
		g_Enemy[i].moveVec = { 0.0f, 0.0f, 0.0f };
		g_Enemy[i].frameRot = 0.0f;
		g_Enemy[i].life = 1;
		g_Enemy[i].lifeMax = g_Enemy[i].life;
		g_Enemy[i].atInterval = 0.0f;
		g_Enemy[i].atBullet = FALSE;
		g_Enemy[i].character = rand() % MAX;
		ChangeDiffuse(i);

		g_Enemy[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Enemy[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		//パーツの設定
		for (int k = 0; k < ENEMY_PARTS_NUM; k++)		//
		{
			int partsNum = i * ENEMY_PARTS_NUM + k;		//パーツの添え字を求める。現在はエネミーの添え字がそのまま来るようになっている
			g_Parts[partsNum].use = TRUE;
			g_Parts[partsNum].parent = &g_Enemy[i];		// 親をセット

			if (k == 0)
			{
				g_Parts[partsNum].tbl_adr = move_tbl_parts;		// 再生するアニメデータの先頭アドレスをセット
				g_Parts[partsNum].tbl_size = sizeof(move_tbl_parts) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
				g_Parts[partsNum].load = TRUE;
			}
		}
		break;
	}
}


void ChangeDiffuse(int i)
{
	//性格ごとに色を上書き
	if (g_Enemy[i].character == PLANE)
	{
		SetModelDiffuse(&g_Enemy[i].model, 0, { 1.0f, 1.0f, 1.0f ,1.0f });
	}
	else if (g_Enemy[i].character == OFFENSIVE)
	{
		SetModelDiffuse(&g_Enemy[i].model, 0, { 2.0f, 0.0f, 0.0f ,1.0f });
	}
	else if (g_Enemy[i].character == TAKE_TIME)
	{
		SetModelDiffuse(&g_Enemy[i].model, 0, { 0.0f, 0.0f, 0.0f ,1.0f });
	}
	else if (g_Enemy[i].character == CROW)
	{
		SetModelDiffuse(&g_Enemy[i].model, 0, { 2.0f, 2.0f, 0.0f ,1.0f });
	}

}

//弾丸攻撃に関する関数
void AttackBullet(int i)
{
	PLAYER *player = GetPlayer();
	ROCK *rock = GetRock();
	CLONE *clone = GetClone();
	if (g_Enemy[i].atBullet == FALSE)
		g_Enemy[i].atInterval += 1.0f;
	//一定時間で弾を再セット
	if (g_Enemy[i].atInterval > AT_INTERVAL)
	{
		g_Enemy[i].atBullet = TRUE;
	}
	if (g_Enemy[i].atBullet != TRUE)return;

	float point = 10000.0f;					//比較用に保存するための変数
	float lenSq = 0.0f;						//距離の大小を図る為に保存する変数
	XMFLOAT3 setPos = {0.0f, 0.0f, 0.0f};	//差分座標を保存するための変数
	//弾がセットされているなら発射対象を選択(探査順序はプレイヤー→クローン)
	//射線が通っているなら座標と距離を保存
	if (g_Enemy[i].atBullet == TRUE && GetCrossPoints(g_Enemy[i].pos, player->pos, rock[i].pos, ROCK_SIZE) == FALSE)
	{
		XMVECTOR pos = XMLoadFloat3(&player->pos) - XMLoadFloat3(&g_Enemy[i].pos);
		XMStoreFloat(&lenSq, XMVector3LengthSq(pos));
		XMStoreFloat3(&setPos, pos);
	}

	for (int k = 0; k < MAX_CLONE; k++)
	{
		if (GetCrossPoints(g_Enemy[i].pos, clone[k].pos, rock[i].pos, ROCK_SIZE) == FALSE)
		{
			XMVECTOR pos = XMLoadFloat3(&clone[k].pos) - XMLoadFloat3(&g_Enemy[i].pos);
			XMStoreFloat(&lenSq, XMVector3LengthSq(pos));

			if (point > lenSq)//距離の大小を計る。距離が小さいほうに狙いを定める
			{
				point = lenSq;
				XMStoreFloat3(&setPos, pos);
			}
		}
	}
	//対象を決めたらそこに弾を発射
	float angle = atan2f(setPos.x, setPos.z);
	SetBullet(g_Enemy[i].pos, XMFLOAT3(0.0f, angle, 0.0f));
	g_Enemy[i].atBullet = FALSE;
	g_Enemy[i].atInterval = 0.0f;

}