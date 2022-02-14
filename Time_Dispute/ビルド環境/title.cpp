//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : �đq�r�N
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_TITLE_MAX			(7)				// �^�C�g���e�N�X�`���̐�
#define TEXTURE_TUTORIAL_MAX		(5)				// �`���[�g���A���e�N�X�`���̐�
#define TEXTURE_MAX					(TEXTURE_TITLE_MAX + TEXTURE_TUTORIAL_MAX)	// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(720)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 
#define	POS_X_CAM			(50.0f)					// �J�����̏����ʒu(X���W)
#define	POS_Y_CAM			(15.0f)					// �J�����̏����ʒu(Y���W)
#define	POS_Z_CAM			(-150.0f)				// �J�����̏����ʒu(Z���W)
#define	ROT_X_CAM			(XM_PI * 1.0f)			// �J�����̏����ʒu(X��])
#define	ROT_Y_CAM			(XM_PI)					// �J�����̏����ʒu(Y��])
#define	ROT_Z_CAM			(-XM_PI * 0.5f)					// �J�����̏����ʒu(Z��])
#define	AT_X_CAM			(-40.0f)					// �J�����̏��������_�ύX��(X���W)
#define	AT_Y_CAM			(50.0f)					// �J�����̏��������_�ύX��(Y���W)
#define	AT_Z_CAM			(0.0f)					// �J�����̏��������_�ύX��(Z���W)
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

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


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static BOOL						g_Load = FALSE;

int								g_TutorialNo;				//�`���[�g���A���e�N�X�`���ԍ�
int								g_SentNo;					// �����e�N�X�`���ԍ�
float							alpha;
BOOL							flag_alpha;
BOOL							flagMode;

//�J�����Ɋւ�����`�⊮�f�[�^
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl(�����_�ύX��), frame
	{ XMFLOAT3(POS_X_CAM, POS_Y_CAM, POS_Z_CAM), XMFLOAT3(ROT_X_CAM, ROT_Y_CAM, ROT_Z_CAM), XMFLOAT3(AT_X_CAM, AT_Y_CAM, AT_Z_CAM), 60 * 1 },
	{ XMFLOAT3(GAME_X_CAM, GAME_Y_CAM, GAME_Z_CAM), XMFLOAT3(-ROT_X_CAM, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 60 * 1 },
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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
	g_Pos   = XMFLOAT3(g_w/4, g_h/4, 0.0f);
	g_TexNo = 0;
	g_TutorialNo = TUTORIAL001;
	g_SentNo = PRESS_ENTER;

	alpha = 1.0f;
	flag_alpha = TRUE;
	flagMode = FALSE;
	// BGM�Đ�
	//PlaySound(SOUND_LABEL_BGM_sample000);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateTitle(void)
{
	CAMERA *cam = GetCamera();

	//�`���[�g���A����ʂł̏���
	if (GetPlayMode() == TUTORIAL && (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_A)))
	{
		PlaySound(SOUND_LABEL_SE_button);
		//�S�`���[�g���A����ʂ�\�����I����܂Ńe�N�X�`�������֑J��
		if (g_TutorialNo < TUTORIAL005)
		{
			g_TutorialNo++;
		}
		else if (g_TutorialNo >= TUTORIAL005)
		{
			SetPlayMode(TITLE_NON);
			flagMode = TRUE;
			cam->tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			cam->move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
			cam->tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			return;
		}
	}

	if (GetPlayMode() != TITLE_NON)
		return;

	//�^�C�g����ʂł̏���

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

	//�J�ڃt���O�������A����L�[���͂���������J�ڃt���O���I���A�J�����ɐ��`�⊮�f�[�^���Z�b�g����B
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
			cam->tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
			cam->move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
			cam->tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		}
		else if (g_SentNo == TUTORIAL_START)
		{
			g_TutorialNo = TUTORIAL001;
			SetPlayMode(TUTORIAL);
		}
	}
	
	if (flagMode != FALSE)return;//�t���O���[�h���Z�b�g���ꂽ�Ȃ�J�ڂ��J�n���Ă���̂ňȍ~�̏������X�L�b�v

	//�^�C�g����ʂł̃v���C���[�h�J�ڐ�̕ύX
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

	if (flag_alpha == TRUE)	//�t�F�[�h����
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

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	if(GetPlayMode() == TUTORIAL)
	{
		g_w = TEXTURE_WIDTH * 0.75;
		g_h = TEXTURE_HEIGHT * 0.75;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.5f, TEXTURE_HEIGHT * 0.5f, 0.0f);
		g_TexNo = g_TutorialNo;
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	if (GetPlayMode() != TITLE_NON || g_TutorialNo >= TUTORIAL003 || flagMode == TRUE)
		return;

	// �^�C�g���̃��S��`��
	{
		g_w = TEXTURE_WIDTH_LOGO;
		g_h = TEXTURE_HEIGHT_LOGO;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.4f, TEXTURE_HEIGHT * 0.25f, 0.0f);
		g_TexNo = TITLE_LOGO;
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	if (g_SentNo != ARROW_L)	//�����e�N�X�`�������ׂĕ\�����I����܂ł͎��s
	{
		// �����e�N�X�`����`��
		g_w = SENTENCE_WIDTH;
		g_h = SENTENCE_HEIGHT;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.5f, TEXTURE_HEIGHT * 0.75f, 0.0f);
		g_TexNo = g_SentNo;
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		if (g_SentNo == PRESS_ENTER)return;

		g_w = SENTENCE_HEIGHT;
		g_h = SENTENCE_HEIGHT;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.20f, TEXTURE_HEIGHT * 0.75f, 0.0f);
		g_TexNo = ARROW_L;
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
		g_w = SENTENCE_HEIGHT;
		g_h = SENTENCE_HEIGHT;
		g_Pos = XMFLOAT3(TEXTURE_WIDTH * 0.80f, TEXTURE_HEIGHT * 0.75f, 0.0f);
		g_TexNo = ARROW_R;
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}





