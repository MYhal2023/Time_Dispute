//=============================================================================
//
// マニー処理 [money.cpp]
// Author : 米倉睦起
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "money.h"
#include "sprite.h"
#include "player.h"
#include "time.h"
#include "game.h"
#include "score.h"
#include "time.h"
#include "life.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(25)	// テクスチャサイズ
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/number16x32.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_Money;					// スコア

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMoney(void)
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


	// マニーの初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 0.0f + TEXTURE_WIDTH * MONEY_DIGIT, 0.0f + ((TIME_TEXTURE_HEIGHT + LIFE_DH) * 1.2f), 0.0f };
	g_TexNo = 0;
	g_Money = 0;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMoney(void)
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
void UpdateMoney(void)
{
	if (GetPlayMode() == TUTORIAL_GAME)
		g_Money = 99999;	// チュートリアルはカンストさせる


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMoney(void)
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

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// 桁数分処理する
	int number = g_Money;
	for (int i = 0; i < MONEY_DIGIT; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// スコアの位置やテクスチャー座標を反映
		float px = g_Pos.x - g_w*i;	// スコアの表示位置X
		float py = g_Pos.y;			// スコアの表示位置Y
		float pw = g_w;				// スコアの表示幅
		float ph = g_h;				// スコアの表示高さ

		float tw = 1.0f / 10;		// テクスチャの幅
		float th = 1.0f / 1;		// テクスチャの高さ
		float tx = x * tw;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// 次の桁へ
		number /= 10;
	}
}


//=============================================================================
// 所持金を加算する
// 引数:add :追加する点数。マイナスも可能(スコア自体は0～99999）
//=============================================================================
void AddMoney(int add)
{
	if (GetPlayMode() == TUTORIAL_GAME)return;	//チュートリアルはスコア加算無し

	PLAYER *player = GetPlayer();
	//ゲームオーバーなら処理をスキップ
	if (GetTime() <= 0 || player->life <= 0)
		return;

	g_Money += add;

	if (g_Money > MONEY_MAX)
	{
		g_Money = MONEY_MAX;

	}
	else if (g_Money < MONEY_MIN)
	{
		g_Money = MONEY_MIN;
	}
}


int GetMoney(void)//現在の所持金を返す関数
{
	return g_Money;
}

