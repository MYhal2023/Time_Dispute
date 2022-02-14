//=============================================================================
//
// 時間延長アイテム処理 [extension.cpp]
// Author : 米倉睦起
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "game.h"
#include "shadow.h"
#include "sound.h"
#include "camera.h"
#include "extension.h"
#include "meshfield.h"
#include "rock.h"
#include "collision.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	EXTEN_WIDTH		(13.0f)			// 頂点サイズ
#define	EXTEN_HEIGHT	(14.0f)			// 頂点サイズ

#define	EXTEN_SPEED		(5.0f)			// アイテムの移動スピード
#define	EXTEN_INTERVAL	(60.0f)			// 
#define EXTEN_AI		(70.0f)				//
#define ITEM_OFFSET_Y	(5.0f)				//
#define ITEM_OFFSET		(20)				// 出現座標調整

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexExten(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static EXTEN						g_Exten[MAX_EXTEN];	// 木ワーク
static int							g_TexNo;				// テクスチャ番号
static float						Pop = 0.0f;
static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/time.png",
};

static BOOL							g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitExten(void)
{
	MakeVertexExten();

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

	// アイテムワークの初期化
	for (int nCntExten = 0; nCntExten < MAX_EXTEN; nCntExten++)
	{
		ZeroMemory(&g_Exten[nCntExten].material, sizeof(g_Exten[nCntExten].material));
		g_Exten[nCntExten].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_Exten[nCntExten].pos = { 0.0f, 0.0f, 0.0f };
		g_Exten[nCntExten].rot = { 0.0f, 0.0f, 0.0f };
		g_Exten[nCntExten].scl = { 1.0f, 1.0f, 1.0f };
		g_Exten[nCntExten].spd = EXTEN_SPEED;
		g_Exten[nCntExten].fWidth = EXTEN_WIDTH;
		g_Exten[nCntExten].fHeight = EXTEN_HEIGHT;
		g_Exten[nCntExten].use = FALSE;

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitExten(void)
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
void UpdateExten(void)
{

	for (int i = 0; i < MAX_EXTEN; i++)
	{
		if (g_Exten[i].use != TRUE)	//使われてないアイテムは処理をスキップ
			continue;

		// レイキャストして足元の高さを求める
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
		XMFLOAT3 hitPosition;								// 交点
		hitPosition.y = g_Exten[i].pos.y - ITEM_OFFSET_Y;	// 外れた時用に初期化しておく
		bool ans = RayHitField(g_Exten[i].pos, &hitPosition, &normal);
		g_Exten[i].pos.y = hitPosition.y + ITEM_OFFSET_Y;

		// 影の位置設定
		SetPositionShadow(g_Exten[i].shadowIdx, XMFLOAT3(g_Exten[i].pos.x, hitPosition.y + 0.2f, g_Exten[i].pos.z));

	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawExten(void)
{
	// αテストを有効に
	SetAlphaTestEnable(TRUE);

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_EXTEN; i++)
	{
		if (g_Exten[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);
			//正規行列の逆行列を算出しビルボード処理を行う
			int inverse = 3;	//転置する正規行列の行列数
			for (int i = 0; i < inverse; i++)
			{
				for (int k = 0; k < inverse; k++)
				{
					mtxWorld.r[i].m128_f32[k] = mtxView.r[k].m128_f32[i];
				}
			}

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Exten[i].scl.x, g_Exten[i].scl.y, g_Exten[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);


			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Exten[i].pos.x, g_Exten[i].pos.y, g_Exten[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Exten[i].mtxWorld, mtxWorld);


			// マテリアル設定
			SetMaterial(g_Exten[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexExten(void)
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

	float fWidth = EXTEN_WIDTH;
	float fHeight = EXTEN_HEIGHT;

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
// アイテムのパラメータをセット
//=============================================================================
int SetExten(void)
{
	Pop += 1.0f;
	int nIdxExten = -1;

	if (Pop >= EXTEN_INTERVAL)
	{
		ROCK *rock = GetRock();
		Pop = 0.0f;
		for (int nCntExten = 0; nCntExten < MAX_EXTEN; nCntExten++)
		{
			if (g_Exten[nCntExten].use != FALSE)	//使用済みならスキップ
				continue;

			//出現場所の設定
			XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
			BOOL ans = FALSE;
			float posX;
			float posZ;
			while (ans == FALSE)
			{
				posX = (float)(ITEM_OFFSET + rand() % (FIELD_X - (ITEM_OFFSET * 2 - 1)));
				posZ = (float)(ITEM_OFFSET + rand() % (FIELD_Z - (ITEM_OFFSET * 2 - 1)));
				pos.x = -(FIELD_X * 0.5f) * BLOCK_SIZE + (posX * BLOCK_SIZE);
				pos.y = ITEM_OFFSET_Y;
				pos.z = (FIELD_Z * 0.5f) * BLOCK_SIZE - (posZ * BLOCK_SIZE);
				ans = TRUE;
				for (int k = 0; k < MAX_ROCK; k++)
				{
					if ((CollisionBC(pos, rock[k].pos, EXTEN_WIDTH, rock[k].size) == TRUE))
						ans = FALSE;
				}
			}

			//マップの端に来ない座標間でランダムに座標を決定
			g_Exten[nCntExten].pos = { pos.x, pos.y, pos.z };
			g_Exten[nCntExten].scl = { 1.0f, 1.0f, 1.0f };
			g_Exten[nCntExten].use = TRUE;
			// 影の設定
			g_Exten[nCntExten].shadowIdx = CreateShadow(g_Exten[nCntExten].pos, 0.2f, 0.2f);
			nIdxExten = nCntExten;

			break;
		}
	}
	return nIdxExten;
}

//=============================================================================
// アイテムの取得
//=============================================================================
EXTEN *GetExten(void)
{
	return &g_Exten[0];
}
