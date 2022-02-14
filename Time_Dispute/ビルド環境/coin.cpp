//=============================================================================
//
// スコアアップコイン処理 [coin.cpp]
// Author : 米倉睦起
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "game.h"
#include "shadow.h"
#include "sound.h"
#include "camera.h"
#include "coin.h"
#include "time.h"
#include "meshfield.h"
#include "rock.h"
#include "collision.h"
#include "clone.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	COIN_WIDTH		(12.0f)			// 頂点サイズ
#define	COIN_HEIGHT		(12.0f)			// 頂点サイズ

#define	COIN_SPEED		(4.2f)			// コインの移動スピード
#define	COIN_INTERVAL	(30.0f)			// 
#define	COIN_AI			(70.0f)			// 
#define ITEM_OFFSET_Y	(5.0f)				// 座標調整
#define ITEM_OFFSET		(20)				// 出現座標調整
//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexCoin(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static COIN						g_Coin[MAX_COIN];	// コインワーク
static int							g_TexNo;				// テクスチャ番号
static float						Pop = 0.0f;
static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/coin.png",
};

static BOOL							g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCoin(void)
{
	MakeVertexCoin();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// コインワークの初期化
	for (int nCntCoin = 0; nCntCoin < MAX_COIN; nCntCoin++)
	{
		ZeroMemory(&g_Coin[nCntCoin].material, sizeof(g_Coin[nCntCoin].material));
		g_Coin[nCntCoin].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_Coin[nCntCoin].pos = { 0.0f, 0.0f, 0.0f };
		g_Coin[nCntCoin].rot = { 0.0f, 0.0f, 0.0f };
		g_Coin[nCntCoin].scl = { 1.0f, 1.0f, 1.0f };
		g_Coin[nCntCoin].spd = COIN_SPEED;
		g_Coin[nCntCoin].fWidth = COIN_WIDTH;
		g_Coin[nCntCoin].fHeight = COIN_HEIGHT;
		g_Coin[nCntCoin].use = FALSE;

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCoin(void)
{
	if (g_Load == FALSE) return;

	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCoin(void)
{

	for (int i = 0; i < MAX_COIN; i++)
	{
		if (g_Coin[i].use != TRUE)	//使われてないバレットは処理をスキップ
			continue;

		// レイキャストして足元の高さを求める
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
		XMFLOAT3 hitPosition;								// 交点
		hitPosition.y = g_Coin[i].pos.y - ITEM_OFFSET_Y;	// 外れた時用に初期化しておく
		bool ans = RayHitField(g_Coin[i].pos, &hitPosition, &normal);
		g_Coin[i].pos.y = hitPosition.y + ITEM_OFFSET_Y;


		// 影の位置設定
		SetPositionShadow(g_Coin[i].shadowIdx, XMFLOAT3(g_Coin[i].pos.x, hitPosition.y + 0.2f, g_Coin[i].pos.z));

	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCoin(void)
{
	// αテストを有効に
	SetAlphaTestEnable(TRUE);

	// ライティングを無効
	SetLightEnable(FALSE);

	//// 加算合成に設定
	//SetBlendState(BLEND_MODE_ADD);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();
	LIGHT *light = GetLight();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_COIN; i++)
	{
		if (g_Coin[i].use == FALSE)continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		mtxView = XMLoadFloat4x4(&cam->mtxView);
		int inverse = 3;	//転置する正規行列の行列数
		for (int i = 0; i < inverse; i++)
		{
			for (int k = 0; k < inverse; k++)
			{
				mtxWorld.r[i].m128_f32[k] = mtxView.r[k].m128_f32[i];
			}
		}

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Coin[i].scl.x, g_Coin[i].scl.y, g_Coin[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);


		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Coin[i].pos.x, g_Coin[i].pos.y, g_Coin[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Coin[i].mtxWorld, mtxWorld);


		// マテリアル設定
		SetMaterial(g_Coin[i].material);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// ポリゴンの描画
		GetDeviceContext()->Draw(4, 0);
	}

	//// 通常ブレンドに戻す
	//SetBlendState(BLEND_MODE_ALPHABLEND);

	// ライティングを有効に
	SetLightEnable(TRUE);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexCoin(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = COIN_WIDTH;
	float fHeight = COIN_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// コインのパラメータをセット
//=============================================================================
int SetCoin(void)
{
	Pop += 1.0f;
	int nIdxCoin = -1;

	if (Pop >= COIN_INTERVAL)
	{
		ROCK *rock = GetRock();
		Pop = 0.0f;
		for (int nCntCoin = 0; nCntCoin < MAX_COIN; nCntCoin++)	//未使用の配列番号にアクセス
		{
			if (g_Coin[nCntCoin].use != FALSE)	//使用済みならスキップ
				continue;

			//出現場所の設定
			//マップの端に来ない座標間でランダムに座標を決定
			XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
			BOOL ans = FALSE;
			while (ans == FALSE)
			{
				float posX;
				float posZ;
				posX = (float)(ITEM_OFFSET + rand() % (FIELD_X - (ITEM_OFFSET * 2 - 1)));
				posZ = (float)(ITEM_OFFSET + rand() % (FIELD_Z - (ITEM_OFFSET * 2 - 1)));
				pos.x = -(FIELD_X * 0.5f) * BLOCK_SIZE + (posX * BLOCK_SIZE);
				pos.y = ITEM_OFFSET_Y;
				pos.z = (FIELD_Z * 0.5f) * BLOCK_SIZE - (posZ * BLOCK_SIZE);
				ans = TRUE;
				for (int k = 0; k < MAX_ROCK; k++)
				{
					if ((CollisionBC(pos, rock[k].pos, COIN_WIDTH + CLONE_SIZE, rock[k].size) == TRUE))
						ans = FALSE;
				}
			}
			g_Coin[nCntCoin].pos = { pos.x, pos.y, pos.z };
			g_Coin[nCntCoin].scl = { 1.0f, 1.0f, 1.0f };
			g_Coin[nCntCoin].use = TRUE;

			// 影の設定
			g_Coin[nCntCoin].shadowIdx = CreateShadow(g_Coin[nCntCoin].pos, 0.2f, 0.2f);

			nIdxCoin = nCntCoin;
			break;
			
		}
	}
	return nIdxCoin;
}

//=============================================================================
// コインの取得
//=============================================================================
COIN *GetCoin(void)
{
	return &g_Coin[0];
}
