//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 米倉睦起
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "game.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_TITLE_MAX			(7)				// タイトルテクスチャの数
#define TEXTURE_TUTORIAL_MAX		(5)				// チュートリアルテクスチャの数
#define TEXTURE_MAX					(TEXTURE_TITLE_MAX + TEXTURE_TUTORIAL_MAX)	// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(720)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 
#define	POS_X_CAM			(50.0f)					// カメラの初期位置(X座標)
#define	POS_Y_CAM			(15.0f)					// カメラの初期位置(Y座標)
#define	POS_Z_CAM			(-150.0f)				// カメラの初期位置(Z座標)
#define	ROT_X_CAM			(XM_PI * 1.0f)			// カメラの初期位置(X回転)
#define	ROT_Y_CAM			(XM_PI)					// カメラの初期位置(Y回転)
#define	ROT_Z_CAM			(-XM_PI * 0.5f)					// カメラの初期位置(Z回転)
#define	AT_X_CAM			(-40.0f)					// カメラの初期注視点変更量(X座標)
#define	AT_Y_CAM			(50.0f)					// カメラの初期注視点変更量(Y座標)
#define	AT_Z_CAM			(0.0f)					// カメラの初期注視点変更量(Z座標)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title_logo.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/tutorial001.PNG",
	"data/TEXTURE/tutorial002.PNG",
	"data/TEXTURE/tutorial003.PNG",
	"data/TEXTURE/tutorial004.PNG",
	"data/TEXTURE/tutorial005.PNG",
	"data/TEXTURE/t_PRESS_ENTER_KEY.png",
	"data/TEXTURE/t_GAME_START.png",
	"data/TEXTURE/t_TUTORIAL_MODE.png",
	"data/TEXTURE/arrow_left.png",
	"data/TEXTURE/arrow_right.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static BOOL						g_Load = FALSE;

int								g_TutorialNo;				//チュートリアルテクスチャ番号
int								g_SentNo;					// 文字テクスチャ番号
float							alpha;
BOOL							flag_alpha;
BOOL							flagMode;

//カメラに関する線形補完データ
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl(注視点変更量), frame
	{ XMFLOAT3(POS_X_CAM, POS_Y_CAM, POS_Z_CAM), XMFLOAT3(ROT_X_CAM, ROT_Y_CAM, ROT_Z_CAM), XMFLOAT3(AT_X_CAM, AT_Y_CAM, AT_Z_CAM), 60 * 1 },
	{ XMFLOAT3(GAME_X_CAM, GAME_Y_CAM, GAME_Z_CAM), XMFLOAT3(-ROT_X_CAM, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 60 * 1 },
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/4, g_h/4, 0.0f);
	g_TexNo = 0;
	g_TutorialNo = TUTORIAL001;
	g_SentNo = PRESS_ENTER;

	alpha = 1.0f;
	flag_alpha = TRUE;
	flagMode = FALSE;
	// BGM再生
	//PlaySound(SOUND_LABEL_BGM_sample000);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	CAMERA *cam = GetCamera();

	//チュートリアル画面での処理
	if (GetPlayMode() == TUTORIAL && (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_A)))
	{
		PlaySound(SOUND_LABEL_SE_button);
		//全チュートリアル画面を表示し終えるまでテクスチャを次へ遷移
		if (g_TutorialNo < TUTORIAL005)
		{
			g_TutorialNo++;
		}
		else if (g_TutorialNo >= TUTORIAL005)
		{
			SetPlayMode(TITLE_NON);
			flagMode = TRUE;
			cam->tbl_adr = move_tbl;		// 再生するアニメデータの先頭アドレスをセット
			cam->move_time = 0.0f;	// 線形補間用のタイマーをクリア
			cam->tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
			return;
		}
	}

	if (GetPlayMode() != TITLE_NON)
		return;

	//タイトル画面での処理

	if (flagMode == TRUE && cam->tbl_adr == NULL)
	{
		if (g_SentNo == GAME_START)
		{
			SetPlayMode(MAIN_GAME);
			flagMode = FALSE;
			StopSound();
			PlaySound(SOUND_LABEL_BGM_game);
			return;
		}
		else if (g_TutorialNo >= TUTORIAL005)
		{
			SetPlayMode(TUTORIAL_GAME);
			flagMode = FALSE;
			StopSound();
			PlaySound(SOUND_LABEL_BGM_game);
			return;
		}
	}

	if (flagMode == FALSE)
	{
		cam->pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
		cam->rot = { ROT_X_CAM, ROT_Y_CAM, ROT_Z_CAM };
		cam->atPos = { AT_X_CAM, AT_Y_CAM, AT_Z_CAM };
	}

	//遷移フラグが無く、決定キー入力があったら遷移フラグをオン、カメラに線形補完データをセットする。
	if (flagMode == FALSE && (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_A)))
	{
		PlaySound(SOUND_LABEL_SE_button);
		if (g_SentNo == PRESS_ENTER)
		{
			g_SentNo++;
		}
		else if (g_SentNo == GAME_START)
		{
			flagMode = TRUE;
			cam->tbl_adr = move_tbl;		// 再生するアニメデータの先頭アドレスをセット
			cam->move_time = 0.0f;	// 線形補間用のタイマーをクリア
			cam->tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		}
		else if (g_SentNo == TUTORIAL_START)
		{
			g_TutorialNo = TUTORIAL001;
			SetPlayMode(TUTORIAL);
		}
	}
	
	if (flagMode != FALSE)return;//フラグモードがセットされたなら遷移が開始しているので以降の処理をスキップ

	//タイトル画面でのプレイモード遷移先の変更
	if (g_SentNo < TUTORIAL_START && (GetKeyboardTrigger(DIK_D) || GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT) || IsButtonTriggered(0, BUTTON_R)))
	{
		g_SentNo++;
		PlaySound(SOUND_LABEL_SE_button);
	}
	else if (g_SentNo > GAME_START && (GetKeyboardTrigger(DIK_A) || GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT) || IsButtonTriggered(0, BUTTON_L)))
	{
		g_SentNo--;
		PlaySound(SOUND_LABEL_SE_button);
	}

	if (flag_alpha == TRUE)	//フェード処理
	{
		alpha -= 0.04f;
		if (alpha <= 0.0f)
		{
			alpha = 0.0f;
			flag_alpha = FALSE;
		}
	}
	else
	{
		alpha += 0.02f;
		if (alpha >= 1.0f)
		{
			alpha = 1.0f;
			flag_alpha = TRUE;
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	if(GetPlayMode() == TUTORIAL)
	{
		g_w = TEXTURE_WIDTH * 0.75;
		g_h = TEXTURE_HEIGHT * 0.75;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.5f, TEXTURE_HEIGHT * 0.5f, 0.0f);
		g_TexNo = g_TutorialNo;
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	if (GetPlayMode() != TITLE_NON || g_TutorialNo >= TUTORIAL003 || flagMode == TRUE)
		return;

	// タイトルのロゴを描画
	{
		g_w = TEXTURE_WIDTH_LOGO;
		g_h = TEXTURE_HEIGHT_LOGO;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.4f, TEXTURE_HEIGHT * 0.25f, 0.0f);
		g_TexNo = TITLE_LOGO;
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	if (g_SentNo != ARROW_L)	//文字テクスチャをすべて表示し終えるまでは実行
	{
		// 文字テクスチャを描画
		g_w = SENTENCE_WIDTH;
		g_h = SENTENCE_HEIGHT;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.5f, TEXTURE_HEIGHT * 0.75f, 0.0f);
		g_TexNo = g_SentNo;
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		if (g_SentNo == PRESS_ENTER)return;

		g_w = SENTENCE_HEIGHT;
		g_h = SENTENCE_HEIGHT;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.20f, TEXTURE_HEIGHT * 0.75f, 0.0f);
		g_TexNo = ARROW_L;
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
		g_w = SENTENCE_HEIGHT;
		g_h = SENTENCE_HEIGHT;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.80f, TEXTURE_HEIGHT * 0.75f, 0.0f);
		g_TexNo = ARROW_R;
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}





