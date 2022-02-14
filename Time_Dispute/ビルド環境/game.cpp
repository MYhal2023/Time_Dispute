//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 米倉睦起
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "light.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "title.h"
#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "rock.h"
#include "bullet.h"
#include "score.h"
#include "interface.h"
#include "life.h"
#include "time.h"
#include "coin.h"
#include "extension.h"
#include "amadeus.h"
#include "collision.h"
#include "debugproc.h"
#include "salieri.h"
#include "clone.h"
#include "money.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF
static int	g_PlayMode = TITLE_NON;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	switch (GetMode())
	{
	case MODE_TITLE:
	case MODE_GAME:
		g_ViewPortType_Game = TYPE_FULL_SCREEN;

		InitTitle();

		// フィールドの初期化
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), FIELD_X, FIELD_Z, BLOCK_SIZE, BLOCK_SIZE, WATER);

		// ライトを有効化	// 影の初期化処理
		InitShadow();

		// プレイヤーの初期化
		InitPlayer();

		// エネミーの初期化
		InitEnemy();

		// 壁の初期化
		InitMeshWall(XMFLOAT3(0.0f, WAVE_POS_Y, FIELD_Z_LIMIT), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(-FIELD_X_LIMIT, WAVE_POS_Y, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(FIELD_X_LIMIT, WAVE_POS_Y, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(0.0f, WAVE_POS_Y, -FIELD_Z_LIMIT), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);

		// 壁(裏側用の半透明)
		InitMeshWall(XMFLOAT3(0.0f, WAVE_POS_Y, FIELD_Z_LIMIT), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(-FIELD_X_LIMIT, WAVE_POS_Y, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(FIELD_X_LIMIT, WAVE_POS_Y, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(0.0f, WAVE_POS_Y, -FIELD_Z_LIMIT), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);

		//岩を出す
		InitRock();

		// 弾の初期化
		InitBullet();

		// スコアの初期化
		InitScore();

		// 所持金の初期化
		InitMoney();

		// ライフの初期化
		InitLife();

		// 制限時間の初期化
		InitTime();

		//UI表示初期化
		InitInterface();

		//コインの初期化
		InitCoin();

		//延長アイテムの初期化
		InitExten();

		//敵AI初期化
		InitAmadeus();

		//味方クローン初期化
		InitClone();

		//味方AI初期化
		InitSalieri();


		//BGM再生
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{

	//UI表示終了処理
	UninitInterface();

	// スコアの終了処理
	UninitScore();

	//所持金の終了処理
	UninitMoney();

	// ライフの終了処理
	UninitLife();

	// 制限時間の終了処理
	UninitTime();

	//コインの終了処理
	UninitCoin();

	//延長アイテムの終了処理
	UninitExten();

	// 弾の終了処理
	UninitBullet();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	//タイトル画面の終了処理
	UninitTitle();

	// 影の終了処理
	UninitShadow();

	UninitSalieri();
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;
	PLAYER *player = GetPlayer();
	UpdateTitle();

	// 地面処理の更新
	UpdateMeshField();

	// 壁処理の更新
	UpdateMeshWall();

	// プレイヤーの更新処理
	UpdatePlayer();

	UpdateLight();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHit();

	if (g_PlayMode <= TUTORIAL)	//タイトルならここまで更新
		return;

	// エネミーの更新処理
	UpdateEnemy();

	UpdateClone();

	//岩の更新処理
	UpdateRock();

	// 弾の更新処理
	UpdateBullet();

	//コインの更新処理
	UpdateCoin();

	//延長アイテムの更新処理
	UpdateExten();

	// スコアの更新処理
	UpdateScore();

	//所持金の更新処理
	UpdateMoney();

	//UI表示更新処理
	UpdateInterface();

	// ライフの更新処理
	UpdateLife();

	// 制限時間の更新処理
	UpdateTime();
	
	CheckModeChange();

	//各オブジェクトを設置
	SetCoin();
	SetExten();
	SetEnemy();
}

//=============================================================================
// 描画処理(カメラ目線)
//=============================================================================
void DrawGame0(void)
{
	PLAYER *player = GetPlayer();
	//SetCamera();
	//シェーダー管理
	//ポストエフェクトをかける場合はここから
	int ans = MODE_PLANE;

	if (GetTime() <= 0 || player->life <= 0)
		ans = MODE_MONO;

	SwapShader(ans);

	// 3Dの物を描画する処理
	//シェーダー切り替え。水面の描画処理
	ans = MODE_SPECULAR;
	if (GetTime() <= 0 || player->life <= 0)
		ans = MODE_MONO;
	SwapShader(ans);

	// 地面の描画処理
	DrawMeshField();
	//シェーダー切り替え。ノーマルの物に戻す
	ans = MODE_PLANE;
	if (GetTime() <= 0 || player->life <= 0)
		ans = MODE_MONO;

	SwapShader(ans);

	// 影の描画処理
	DrawShadow();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	DrawClone();

	// 弾の描画処理
	DrawBullet();

	//岩の描画処理
	DrawRock();

	//コインの描画処理
	DrawCoin();

	//延長アイテムの描画処理
	DrawExten();

	// 壁の描画処理
	DrawMeshWall();

	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	DrawTitle();

	if (g_PlayMode > TUTORIAL)
	{
		// スコアの描画処理
		DrawScore();

		//所持金の描画処理
		DrawMoney();

		// ライフの描画処理
		DrawLife();

		// 制限時間の描画処理
		DrawTime();

		//UI表示描画処理
		DrawInterface();
	}

	//シェーダー管理
	//シェーダーを元に戻す。ポストエフェクトはここまで
	ans = MODE_PLANE;
	SwapShader(ans);
	if (GetTime() <= 0 || player->life <= 0)
		DrawGameOver();

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}

//=============================================================================
// 描画処理(ライト目線。生成したい影に関するオブジェクトだけを指定)
//=============================================================================
void DrawGame1(void)
{
	// 地面の描画処理
	DrawMeshField();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	//味方の描画処理
	DrawClone();

	//岩の描画処理
	DrawRock();
}


void DrawGame(void)
{


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif
	XMFLOAT3 pos;

	// プレイヤー視点
	pos = GetPlayer()->pos;
	//pos = GetEnemy()->pos;	//デバッグ用
	pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		DrawGame0();
		break;

	case TYPE_LIGHT_SCREEN:	//ライト目線の描画をするためのビューポートタイプ
		DrawGame1();
		break;

	}

}


//=============================================================================
// オブジェクト同士の当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	BULLET *bullet = GetBullet();	// 弾のポインターを初期化
	COIN *coin = GetCoin();
	EXTEN *exten = GetExten();
	CLONE *clone = GetClone();
	// エネミーとキャラクター
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//エネミーの有効フラグをチェックする
		if (enemy[i].use == FALSE)
			continue;
		//BCの当たり判定
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size) == TRUE)
		{
			if (player->attackUse == TRUE)//プレイヤーが攻撃
			{
				// エネミーを消す
				enemy[i].use = FALSE;
				ReleaseShadow(enemy[i].shadowIdx);
				player->attackUse = FALSE;	//プレイヤーの攻撃を使用済みにする
				AddScore(SCORE_BEAT);
				PlaySound(SOUND_LABEL_SE_enemydown);
				break;
			}
			else if (player->attackUse == FALSE && player->attack == FALSE && player->atInvinc == FALSE)	//攻撃が使用済みかつ攻撃モーション中でなく、無敵時間中でもない
			{
				AddLife(-1, PLAYER_LIFE, 0);	// プレイヤーのライフを減らす
				player->atInvinc = TRUE;		//無敵時間に入る
				PlaySound(SOUND_LABEL_SE_hit000);
				break;
			}
		}
	}

	// エネミーとクローン
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//エネミーの有効フラグをチェックする
		if (enemy[i].use == FALSE)
			continue;
		for (int k = 0; k < MAX_CLONE; k++)
		{
			//BCの当たり判定
			if (CollisionBC(clone[k].pos, enemy[i].pos, clone[k].size, enemy[i].size) != TRUE)
				continue;

			if (clone[k].attackUse == TRUE)			//クローンが攻撃中
			{
				// エネミーを消す
				enemy[i].use = FALSE;
				clone[k].attackUse = FALSE;			//攻撃を使用済みにする
				AddScore((int)(SCORE_BEAT * 0.5));
				//PlaySound(SOUND_LABEL_SE_enemydown);
				break;
			}
			else if (clone[k].attackUse == FALSE && clone[k].attack == FALSE && clone[k].atInvinc == FALSE)	//攻撃が使用済みかつ攻撃モーション中でなく、無敵時間中でもない
			{
				AddLife(-1, CLONE_LIFE, k);			//ライフを減らす
				clone[k].atInvinc = TRUE;			//無敵時間に入る
				//PlaySound(SOUND_LABEL_SE_hit000);
				break;
			}

		}
	}
	// エネミーの弾とキャラクター
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (bullet[i].use == FALSE)
			continue;
		//BCの当たり判定、無敵中なら処理をスキップ
		if (CollisionBC(bullet[i].pos, player->pos, bullet[i].fWidth, player->size) == TRUE && player->atInvinc == FALSE)
		{
			// 当たったから未使用に戻す
			bullet[i].use = FALSE;
			ReleaseShadow(bullet[i].shadowIdx);
			AddLife(-1, PLAYER_LIFE, 0);	//プレイヤーのライフを減らす
			player->atInvinc = TRUE;		//無敵時間に入る
			PlaySound(SOUND_LABEL_SE_hit000);
			break;
		}
		//クローンをチェック
		for (int k = 0; k < MAX_CLONE; k++)
		{
			if (CollisionBC(bullet[i].pos, clone[k].pos, bullet[i].fWidth, clone[k].size) != TRUE || clone[k].atInvinc != FALSE)
				continue;

			// 当たったから未使用に戻す
			bullet[i].use = FALSE;
			ReleaseShadow(bullet[i].shadowIdx);
			AddLife(-1, CLONE_LIFE, k);	//プレイヤーのライフを減らす
			clone[k].atInvinc = TRUE;		//無敵時間に入る
			//PlaySound(SOUND_LABEL_SE_hit000);
			break;
		}
	}

	// コインとキャラクター
	for (int i = 0; i < MAX_COIN; i++)
	{
		//コインの有効フラグをチェックする
		if (coin[i].use == FALSE)
			continue;
		//コインとプレイヤー
		if (CollisionBC(coin[i].pos, player->pos, coin[i].fWidth, player->size) == TRUE)
		{
			// 当たったから未使用に戻す
			coin[i].use = FALSE;
			ReleaseShadow(coin[i].shadowIdx);
			// プレイヤーのスコアと所持金を増やす
			AddScore(SCORE_COIN);
			AddMoney(10);
			PlaySound(SOUND_LABEL_SE_coin);
			break;
		}

		//続いてクローンをチェック
		for (int k = 0; k < MAX_CLONE; k++)
		{
			if (CollisionBC(coin[i].pos, clone[k].pos, coin[i].fWidth, clone[k].size) != TRUE)
				continue;

			// 当たったから未使用に戻す
			coin[i].use = FALSE;
			ReleaseShadow(coin[i].shadowIdx);
			// プレイヤーのスコアと所持金を増やす
			AddScore((int)(SCORE_COIN * 0.5));
			AddMoney(10);
			//PlaySound(SOUND_LABEL_SE_coin);
			break;
		}
		//最後にエネミーをチェック
		for (int n = 0; n < MAX_ENEMY; n++)
		{
			//エネミーの有効フラグをチェックする
			if (enemy[n].use == FALSE)
				continue;

			//BCの当たり判定
			if (CollisionBC(coin[i].pos, enemy[n].pos, coin[i].fWidth, enemy[n].size) != TRUE)
				continue;

			// 当たったから未使用に戻す
			coin[i].use = FALSE;
			ReleaseShadow(coin[i].shadowIdx);	//影の解放処理
			// プレイヤーのスコアを減らす
			AddScore(-(int)(SCORE_COIN * 0.5));
			AddMoney(-5);
			PlaySound(SOUND_LABEL_SE_coin_enemy);
			break;
		}

	}

	// 時間延長アイテムとキャラクター
	for (int i = 0; i < MAX_EXTEN; i++)
	{
		//時間延長アイテムの有効フラグをチェックする
		if (exten[i].use == FALSE)
			continue;

		//BCの当たり判定
		if (CollisionBC(exten[i].pos, player->pos, exten[i].fWidth, player->size) == TRUE)
		{
			// 当たったから未使用に戻す
			exten[i].use = FALSE;
			ReleaseShadow(exten[i].shadowIdx);
			// 制限時間の延長
			AddTime((int)(ADD_TIME * 2));
			PlaySound(SOUND_LABEL_SE_clock);
			break;
		}
		//続いてクローンをチェック
		for (int k = 0; k < MAX_CLONE; k++)
		{
			if (CollisionBC(exten[i].pos, clone[k].pos, exten[i].fWidth, clone[k].size) != TRUE)
				continue;

			// 当たったから未使用に戻す
			exten[i].use = FALSE;
			ReleaseShadow(exten[i].shadowIdx);
			// 制限時間の延長
			AddTime(ADD_TIME);
			//PlaySound(SOUND_LABEL_SE_clock);
			break;
		}
		//最後にエネミーをチェック
		for (int n = 0; n < MAX_ENEMY; n++)
		{
			//エネミーの有効フラグをチェックする
			if (enemy[n].use == FALSE)
				continue;

			//BCの当たり判定
			if (CollisionBC(exten[i].pos, enemy[n].pos, exten[i].fWidth, enemy[n].size) != TRUE)
				continue;

			// 当たったから未使用に戻す
			exten[i].use = FALSE;
			ReleaseShadow(exten[i].shadowIdx);	//影の解放処理
			// プレイヤーのスコアを減らす
			AddTime(-ADD_TIME);
			PlaySound(SOUND_LABEL_SE_clock_enemy);
			break;
		}
	}
}

void CheckModeChange(void)
{
	PLAYER *player = GetPlayer();
	if (GetPlayMode() == TUTORIAL_GAME)
	{
		if (GetKeyboardTrigger(DIK_X) || IsButtonTriggered(0, BUTTON_B))
		{
			SetFade(FADE_OUT, MODE_RESULT);
		}
	}


	//時間切れかライフ切れでリザルトへ
	if (GetTime() > 0 && player->life > 0)	
		return;

	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}
}

void SetPlayMode(int playMode)
{
	g_PlayMode = playMode;
}

int GetPlayMode(void)
{
	return g_PlayMode;
}

void SetViewPortType(int viewport)
{
	g_ViewPortType_Game = viewport;
}

int GetViewPortTypeGame(void)
{
	return g_ViewPortType_Game;
}