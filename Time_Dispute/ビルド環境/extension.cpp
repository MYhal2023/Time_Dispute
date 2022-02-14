//=============================================================================
//
// ���ԉ����A�C�e������ [extension.cpp]
// Author : �đq�r�N
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	EXTEN_WIDTH		(13.0f)			// ���_�T�C�Y
#define	EXTEN_HEIGHT	(14.0f)			// ���_�T�C�Y

#define	EXTEN_SPEED		(5.0f)			// �A�C�e���̈ړ��X�s�[�h
#define	EXTEN_INTERVAL	(60.0f)			// 
#define EXTEN_AI		(70.0f)				//
#define ITEM_OFFSET_Y	(5.0f)				//
#define ITEM_OFFSET		(20)				// �o�����W����

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexExten(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static EXTEN						g_Exten[MAX_EXTEN];	// �؃��[�N
static int							g_TexNo;				// �e�N�X�`���ԍ�
static float						Pop = 0.0f;
static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/time.png",
};

static BOOL							g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitExten(void)
{
	MakeVertexExten();

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

	// �A�C�e�����[�N�̏�����
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
// �I������
//=============================================================================
void UninitExten(void)
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
void UpdateExten(void)
{

	for (int i = 0; i < MAX_EXTEN; i++)
	{
		if (g_Exten[i].use != TRUE)	//�g���ĂȂ��A�C�e���͏������X�L�b�v
			continue;

		// ���C�L���X�g���đ����̍��������߂�
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
		XMFLOAT3 hitPosition;								// ��_
		hitPosition.y = g_Exten[i].pos.y - ITEM_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
		bool ans = RayHitField(g_Exten[i].pos, &hitPosition, &normal);
		g_Exten[i].pos.y = hitPosition.y + ITEM_OFFSET_Y;

		// �e�̈ʒu�ݒ�
		SetPositionShadow(g_Exten[i].shadowIdx, XMFLOAT3(g_Exten[i].pos.x, hitPosition.y + 0.2f, g_Exten[i].pos.z));

	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawExten(void)
{
	// ���e�X�g��L����
	SetAlphaTestEnable(TRUE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_EXTEN; i++)
	{
		if (g_Exten[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);
			//���K�s��̋t�s����Z�o���r���{�[�h�������s��
			int inverse = 3;	//�]�u���鐳�K�s��̍s��
			for (int i = 0; i < inverse; i++)
			{
				for (int k = 0; k < inverse; k++)
				{
					mtxWorld.r[i].m128_f32[k] = mtxView.r[k].m128_f32[i];
				}
			}

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Exten[i].scl.x, g_Exten[i].scl.y, g_Exten[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);


			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Exten[i].pos.x, g_Exten[i].pos.y, g_Exten[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Exten[i].mtxWorld, mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Exten[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexExten(void)
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

	float fWidth = EXTEN_WIDTH;
	float fHeight = EXTEN_HEIGHT;

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
// �A�C�e���̃p�����[�^���Z�b�g
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
			if (g_Exten[nCntExten].use != FALSE)	//�g�p�ς݂Ȃ�X�L�b�v
				continue;

			//�o���ꏊ�̐ݒ�
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

			//�}�b�v�̒[�ɗ��Ȃ����W�ԂŃ����_���ɍ��W������
			g_Exten[nCntExten].pos = { pos.x, pos.y, pos.z };
			g_Exten[nCntExten].scl = { 1.0f, 1.0f, 1.0f };
			g_Exten[nCntExten].use = TRUE;
			// �e�̐ݒ�
			g_Exten[nCntExten].shadowIdx = CreateShadow(g_Exten[nCntExten].pos, 0.2f, 0.2f);
			nIdxExten = nCntExten;

			break;
		}
	}
	return nIdxExten;
}

//=============================================================================
// �A�C�e���̎擾
//=============================================================================
EXTEN *GetExten(void)
{
	return &g_Exten[0];
}
