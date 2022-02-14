//=============================================================================
//
// ���̑�UI�\������ [interface.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "interface.h"
#include "sprite.h"
#include "game.h"
#include "title.h"
#include "clone.h"
#include "money.h"
#include "time.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(1830 * 0.20)	// �e�N�X�`���T�C�Y
#define TEXTURE_HEIGHT				(920 * 0.20)	// 
#define TEXTURE_MAX					(6)				// �e�N�X�`���̐�
#define UI_CLONE_WIDTH				(325 * 0.25f)	// �e�N�X�`���T�C�Y
#define UI_CLONE_HEIGHT				(215 * 0.25f)	// 
#define UI_CLONE_X					(SCREEN_WIDTH * 0.3f)	// �e�N�X�`�����W
#define UI_CLONE_Y					(SCREEN_HEIGHT)					// 


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/KeyExplain.png",
	"data/TEXTURE/t_TUTORIAL_MODE.png",
	"data/TEXTURE/STOP_TUTORIAL.png",
	"data/TEXTURE/clone001.png",
	"data/TEXTURE/clone002.png",
	"data/TEXTURE/clone003.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�


static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitInterface(void)
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
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 720.0f, 440.0f, 0.0f };
	g_TexNo = 0;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitInterface(void)
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
void UpdateInterface(void)
{


#ifdef _DEBUG	// �f�o�b�O����\������
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawInterface(void)
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
	g_TexNo = CONTROL_TEXTURE;
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { 720.0f, 440.0f, 0.0f };

	// UI�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_Pos.x;	// UI�̕\���ʒuX
	float py = g_Pos.y;			// UI�̕\���ʒuY
	float pw = g_w;				// UI�̕\����
	float ph = g_h;				// UI�̕\������

	float tw = 1.0f;		// �e�N�X�`���̕�
	float th = 1.0f;		// �e�N�X�`���̍���
	float tx = 0.0f;			// �e�N�X�`���̍���X���W
	float ty = 0.0f;			// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	//�N���[���Z�b�gUI�̕\��
	for (int i = 0; i < CLONE_MAX; i++)
	{
		g_TexNo = CLONE001 + i;
		PLAYER *player = GetPlayer();
		if ((g_TexNo == CLONE001 && GetMoney() < CLONE_OFFENSIVE_VALUE) ||
			(g_TexNo == CLONE002 && GetMoney() < CLONE_TAKE_TIME_VALUE) ||
			(g_TexNo == CLONE003 && GetMoney() < CLONE_CROW_VALUE) ||
			(GetTime() <= 0 || player->life <= 0))
		{
			SwapShader(MODE_MONO);	//�w���ł��Ȃ��ꍇ�A�V�F�[�_�[�����m�N����
		}

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		g_Pos.x = UI_CLONE_X + (UI_CLONE_WIDTH * i) + i * 8;//���Ԃ��J���`��
		g_Pos.y = UI_CLONE_Y - UI_CLONE_HEIGHT * 0.5f;
		g_w = UI_CLONE_WIDTH;
		g_h = UI_CLONE_HEIGHT;

		// UI�̈ʒu��e�N�X�`���[���W�𔽉f
		px = g_Pos.x;	// UI�̕\���ʒuX
		py = g_Pos.y;			// UI�̕\���ʒuY
		pw = g_w;				// UI�̕\����
		ph = g_h;				// UI�̕\������

		tw = 1.0f;		// �e�N�X�`���̕�
		th = 1.0f;		// �e�N�X�`���̍���
		tx = 0.0f;			// �e�N�X�`���̍���X���W
		ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		SwapShader(MODE_PLANE);//�V�F�[�_�[�����ɖ߂�
	}

	if (GetPlayMode() != TUTORIAL_GAME)return;	//�`���[�g���A�����Ȃ炻��𖾎�����e�N�X�`����\��

	g_TexNo = TUTORIAL_NOW;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	g_Pos.x = SCREEN_WIDTH * 0.25f;
	g_Pos.y = SENTENCE_HEIGHT * 0.5f;
	g_w = SENTENCE_WIDTH;
	g_h = SENTENCE_HEIGHT;

	// UI�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_Pos.x;	// UI�̕\���ʒuX
	py = g_Pos.y;			// UI�̕\���ʒuY
	pw = g_w;				// UI�̕\����
	ph = g_h;				// UI�̕\������

	tw = 1.0f;		// �e�N�X�`���̕�
	th = 1.0f;		// �e�N�X�`���̍���
	tx = 0.0f;			// �e�N�X�`���̍���X���W
	ty = 0.0f;			// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	g_TexNo = STOP_TUTORIAL;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	g_Pos.x = SCREEN_WIDTH * 0.85f;
	g_Pos.y = SENTENCE_HEIGHT;
	g_w = SENTENCE_WIDTH * 0.5;
	g_h = SENTENCE_HEIGHT * 2.0f;

	// UI�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_Pos.x;	// UI�̕\���ʒuX
	py = g_Pos.y;			// UI�̕\���ʒuY
	pw = g_w;				// UI�̕\����
	ph = g_h;				// UI�̕\������

	tw = 1.0f;		// �e�N�X�`���̕�
	th = 1.0f;		// �e�N�X�`���̍���
	tx = 0.0f;			// �e�N�X�`���̍���X���W
	ty = 0.0f;			// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}
