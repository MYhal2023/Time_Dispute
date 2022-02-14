//=============================================================================
//
// �X�R�A�A�b�v�R�C������ [coin.cpp]
// Author : �đq�r�N
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	COIN_WIDTH		(12.0f)			// ���_�T�C�Y
#define	COIN_HEIGHT		(12.0f)			// ���_�T�C�Y

#define	COIN_SPEED		(4.2f)			// �R�C���̈ړ��X�s�[�h
#define	COIN_INTERVAL	(30.0f)			// 
#define	COIN_AI			(70.0f)			// 
#define ITEM_OFFSET_Y	(5.0f)				// ���W����
#define ITEM_OFFSET		(20)				// �o�����W����
//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexCoin(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static COIN						g_Coin[MAX_COIN];	// �R�C�����[�N
static int							g_TexNo;				// �e�N�X�`���ԍ�
static float						Pop = 0.0f;
static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/coin.png",
};

static BOOL							g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitCoin(void)
{
	MakeVertexCoin();

	// �e�N�X�`������
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

	// �R�C�����[�N�̏�����
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
// �I������
//=============================================================================
void UninitCoin(void)
{
	if (g_Load == FALSE) return;

	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateCoin(void)
{

	for (int i = 0; i < MAX_COIN; i++)
	{
		if (g_Coin[i].use != TRUE)	//�g���ĂȂ��o���b�g�͏������X�L�b�v
			continue;

		// ���C�L���X�g���đ����̍��������߂�
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
		XMFLOAT3 hitPosition;								// ��_
		hitPosition.y = g_Coin[i].pos.y - ITEM_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
		bool ans = RayHitField(g_Coin[i].pos, &hitPosition, &normal);
		g_Coin[i].pos.y = hitPosition.y + ITEM_OFFSET_Y;


		// �e�̈ʒu�ݒ�
		SetPositionShadow(g_Coin[i].shadowIdx, XMFLOAT3(g_Coin[i].pos.x, hitPosition.y + 0.2f, g_Coin[i].pos.z));

	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCoin(void)
{
	// ���e�X�g��L����
	SetAlphaTestEnable(TRUE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	//// ���Z�����ɐݒ�
	//SetBlendState(BLEND_MODE_ADD);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();
	LIGHT *light = GetLight();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_COIN; i++)
	{
		if (g_Coin[i].use == FALSE)continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		mtxView = XMLoadFloat4x4(&cam->mtxView);
		int inverse = 3;	//�]�u���鐳�K�s��̍s��
		for (int i = 0; i < inverse; i++)
		{
			for (int k = 0; k < inverse; k++)
			{
				mtxWorld.r[i].m128_f32[k] = mtxView.r[k].m128_f32[i];
			}
		}

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Coin[i].scl.x, g_Coin[i].scl.y, g_Coin[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);


		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Coin[i].pos.x, g_Coin[i].pos.y, g_Coin[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Coin[i].mtxWorld, mtxWorld);


		// �}�e���A���ݒ�
		SetMaterial(g_Coin[i].material);

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// �|���S���̕`��
		GetDeviceContext()->Draw(4, 0);
	}

	//// �ʏ�u�����h�ɖ߂�
	//SetBlendState(BLEND_MODE_ALPHABLEND);

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexCoin(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = COIN_WIDTH;
	float fHeight = COIN_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �R�C���̃p�����[�^���Z�b�g
//=============================================================================
int SetCoin(void)
{
	Pop += 1.0f;
	int nIdxCoin = -1;

	if (Pop >= COIN_INTERVAL)
	{
		ROCK *rock = GetRock();
		Pop = 0.0f;
		for (int nCntCoin = 0; nCntCoin < MAX_COIN; nCntCoin++)	//���g�p�̔z��ԍ��ɃA�N�Z�X
		{
			if (g_Coin[nCntCoin].use != FALSE)	//�g�p�ς݂Ȃ�X�L�b�v
				continue;

			//�o���ꏊ�̐ݒ�
			//�}�b�v�̒[�ɗ��Ȃ����W�ԂŃ����_���ɍ��W������
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

			// �e�̐ݒ�
			g_Coin[nCntCoin].shadowIdx = CreateShadow(g_Coin[nCntCoin].pos, 0.2f, 0.2f);

			nIdxCoin = nCntCoin;
			break;
			
		}
	}
	return nIdxCoin;
}

//=============================================================================
// �R�C���̎擾
//=============================================================================
COIN *GetCoin(void)
{
	return &g_Coin[0];
}
