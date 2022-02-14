//=============================================================================
//
// �������ԏ��� [time.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"
#include "time.h"
#include "fade.h"
#include "game.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(32)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(64)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�
#define TIME_CENT					(2)		// ���ƕb�̒P�ʂ̊Ԃɂ���:
#define TIME_MINUTES				(2)		// ���P�ʂ̐�:
#define TIME_SECONDS				(2)		// �b�P�ʂ̐�:
#define TIME_CENT					(2)		// ���ƕb�̒P�ʂ̊Ԃɂ���:
#define ONE_SECOND					(60)	// ��b�ɂ�����t���[�����[�g���ƁA�ꕪ�ɂ�����b��
#define SCORE_UP_INTERVAL			(5)	// �X�R�A�̑����W���𑝂₷�Ԋu


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/number16x32.png",
	"data/TEXTURE/timer_cent.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_Time;						// ��������
static int						second;						//�b���o�߂̎w�W�B�����ł�60fps
static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTime(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �v���C���[�̏�����
	g_Use   = TRUE;
	g_w     = TIME_TEXTURE_WIDTH;
	g_h     = TIME_TEXTURE_HEIGHT;
	g_Pos   = { 150.0f, 20.0f, 0.0f };
	g_TexNo = 0;

	g_Time = 60;	// �������Ԃ̏�����
#ifdef DEBUG
	g_Time = 1000
#endif 

	second = 0;
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTime(void)
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
// �X�V����
//=============================================================================
void UpdateTime(void)
{
	RemoveTime(1);

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTime(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// ��������������
	int number = g_Time;
	for (int i = 0; i < TIME_DIGIT; i++)
	{
		if (i != 1 && i != TIME_CENT)	//�\�i���̏���
		{
			g_TexNo = NUMBER;
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
			// ����\�����錅�̐���
			float x = (float)(number % 10);

			// �������Ԃ̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Pos.x - g_w * i;	// �������Ԃ̕\���ʒuX
			float py = g_Pos.y;			// �������Ԃ̕\���ʒuY
			float pw = g_w;				// �������Ԃ̕\����
			float ph = g_h;				// �������Ԃ̕\������

			float tw = 1.0f / 10;		// �e�N�X�`���̕�
			float th = 1.0f / 1;		// �e�N�X�`���̍���
			float tx = x * tw;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			// ���̌���
			number /= 10;

		}
		else if (i == 1)	//���Ԃ̓񌅖ڂ̂�6�i���Ȃ��߁A�����𕪂���
		{
			g_TexNo = NUMBER;
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
			// ����\�����錅�̐���
			float x = (float)(number % 6);

			// �������Ԃ̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Pos.x - g_w * i;	// �������Ԃ̕\���ʒuX
			float py = g_Pos.y;			// �������Ԃ̕\���ʒuY
			float pw = g_w;				// �������Ԃ̕\����
			float ph = g_h;				// �������Ԃ̕\������

			float tw = 1.0f / 10;		// �e�N�X�`���̕�
			float th = 1.0f / 1;		// �e�N�X�`���̍���
			float tx = x * tw;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			// ���̌���
			number /= 6;


		}
		else if (i == TIME_CENT)
		{
			g_TexNo = CENT;

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �������Ԃ̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Pos.x - g_w * i;			// �������Ԃ̕\���ʒuX
			float py = g_Pos.y;			// �������Ԃ̕\���ʒuY
			float pw = g_w;				// �������Ԃ̕\����
			float ph = g_h;				// �������Ԃ̕\������

			float tw = 1.0f;		// �e�N�X�`���̕�
			float th = 1.0f;		// �e�N�X�`���̍���
			float tx = 0.0f;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}
	}	
}


//=============================================================================
// �������Ԃ����Z����(1�b�o�߂�1�������Ă���)
// ����:remove ���Ԃ��}�C�i�X���Ă���
//=============================================================================
void RemoveTime(int remove)
{
	if (GetPlayMode() == TUTORIAL_GAME)return;	//�`���[�g���A���̓X�R�A���Z����

	second++;
	if (second >= ONE_SECOND)
	{
		g_Time -= remove;
		second = 0;
		int k = 1;
		int count = g_Time;
		while (count > 0)
		{
			count -= SCORE_UP_INTERVAL;
			k++;
		}
		AddScore(1 * k);
		if (g_Time <= 0)
			g_Time = 0;
	}

}

//=============================================================================
// �������Ԃ����Z����
// ����:add ���Ԃ��v���X���Ă����B�������Ԃ�0�Ȃ�Ή��Z�������s��Ȃ�
//=============================================================================
void AddTime(int add)
{
	if (GetPlayMode() == TUTORIAL_GAME)return;	//�`���[�g���A���̓X�R�A���Z����

	if (g_Time > 0)
	{
		g_Time += add;

		if (g_Time > TIME_MAX)
			g_Time = TIME_MAX;

	}
}

int GetTime(void)
{
	return g_Time;
}