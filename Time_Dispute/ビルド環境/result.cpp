//=============================================================================
//
// ���U���g��ʏ��� [result.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "result.h"
#include "game.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(3)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/result001.jpg",
	"data/TEXTURE/ResultLogo.png",
	"data/TEXTURE/number16x32.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static float					g_DiffuseR, g_DiffuseG, g_DiffuseB;
static int						g_TexNo;					// �e�N�X�`���ԍ�
static int						g_ResultScore;				//���U���g�ɕ\������X�R�A
static BOOL						g_Load = FALSE;
static BOOL						g_LoadData = FALSE;
static BOOL						g_SetScore = FALSE;
static int						g_ScoreNum = 0;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
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


	// �ϐ��̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { g_w / 2, 50.0f, 0.0f };
	g_DiffuseR = 1.0f;
	g_DiffuseG = 1.0f;
	g_DiffuseB = 1.0f;
	g_TexNo = 0;
	g_ResultScore = 0;
	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_result);

	g_Load = TRUE;
	g_LoadData = FALSE;
	g_SetScore = FALSE;
	g_ScoreNum = SCORE_SAVE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{
	if (g_LoadData == FALSE)//���[�h���Ă��Ȃ��Ȃ�f�[�^�����[�h
	{
		//�X�R�A�f�[�^�����[�h
		LoadData();
		g_LoadData = TRUE;

	}

	if (g_SetScore == FALSE)//�Z�[�u�����Ă��Ȃ��Ȃ炷��
	{
		//�X�R�A�f�[�^���X�V
		int num = 0;
		int savenum = 0;
		int score = 0;		//���ɕۑ�����X�R�A��ۑ�����p
		int savescore = 0;	//�ꎞ�ޔ�p
		for (int i = 0; i < SCORE_SAVE; i++)
		{
			if (GetScore() > GetSaveScore(i))	//�n�C�X�R�A�X�V�ӏ����ォ��T��
			{
				score = GetScore();
				savescore = GetSaveScore(i);
				SetSaveScore(score, i);
				g_ScoreNum = i;
				for (num = i + 1; num < SCORE_SAVE; num++)
				{
					savenum = num + 1;
					score = savescore;
					if (savenum < SCORE_SAVE)
						savescore = GetSaveScore(num);

					SetSaveScore(score, num);
				}
				break;
			}
		}
		g_SetScore = TRUE;
		SaveData();
	}
	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_A))
	{// Enter��������A�X�e�[�W��؂�ւ���
		if (GetScore() > GetHighScore())
		{
			SetHighScore(GetScore());
		}
		SetPlayMode(TITLE_NON);
		SetFade(FADE_OUT, MODE_GAME);
	}



#ifdef _DEBUG	// �f�o�b�O����\������
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
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

	// ���U���g�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���U���g�̃��S��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �X�R�A�\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
		for (int k = 0; k < SCORE_SAVE; k++)
		{
			g_ResultScore = GetSaveScore(k);
			int number = g_ResultScore;
			for (int i = 0; i < SCORE_DIGIT; i++)
			{
				// ����\�����錅�̐���
				float x = (float)(number % 10);

				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				float pw = 16 * 1.2f;			// �X�R�A�̕\����
				float ph = 32 * 1.2f;			// �X�R�A�̕\������
				float px = (SCREEN_WIDTH * 0.5f + (pw * 0.5f) * ((float)(SCORE_DIGIT) * 0.5f)) - i * pw;		// �X�R�A�̕\���ʒuX
				float py = 120.0f + (k * ph * 1.1f);// �X�R�A�̕\���ʒuY

				float tw = 1.0f / 10;		// �e�N�X�`���̕�
				float th = 1.0f / 1;		// �e�N�X�`���̍���
				float tx = x * tw;			// �e�N�X�`���̍���X���W
				float ty = 0.0f;			// �e�N�X�`���̍���Y���W
				if (g_ScoreNum == k)
				{
					g_DiffuseR = 1.0f;
					g_DiffuseG = 1.0f;
					g_DiffuseB = 0.0f;
				}
				else
				{
					g_DiffuseR = 1.0f;
					g_DiffuseG = 1.0f;
					g_DiffuseB = 1.0f;
				}
				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(g_DiffuseR, g_DiffuseG, g_DiffuseB, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// ���̌���
				number /= 10;
			}
		}
	}
}




