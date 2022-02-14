//=============================================================================
//
// ���C�t���� [life.cpp]
// Author : �đq �r�N
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "life.h"
#include "sprite.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "game.h"
#include "clone.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(LIFE_DW)	// �n�[�g�̃T�C�Y
#define TEXTURE_HEIGHT				(LIFE_DH)	// 
#define TEXTURE_MAX					(4)			// �e�N�X�`���̐�
#define ALPHA_COUNT					(0.025f)		// ���l�̌�����
#define MINUS_COUNT					(0.01f)	// GB�l�̌�����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/hart.png",
	"data/TEXTURE/hart_lost.png",
	"data/TEXTURE/BoS_life.png",
	"data/TEXTURE/g_GAME_OVER.png",
};

static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static LIFE	g_Life;
float lifeAlpha;
float lifeGB;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitLife(void)
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
	g_Life.w = TEXTURE_WIDTH;
	g_Life.h = TEXTURE_HEIGHT;
	g_Life.pos = XMFLOAT3(LIFE_DX, LIFE_DY, 0.0f);
	g_Life.texNo = 0;
	g_Life.life = 4;	// ���C�t��������

	lifeAlpha = 0.0f;
	lifeGB = 0.0f;
	g_Load = TRUE;		// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitLife(void)
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
void UpdateLife(void)
{

}

//=============================================================================
// �`�揈�� �v���C���[�̃��C�t�͏�ɕ\��������
//=============================================================================
void DrawLife(void)
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

	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	//���`�悷�邩�v�Z����
	g_Life.life = player->life;
	g_Life.life_max = player->lifeMax;
	g_Life.pos = XMFLOAT3(LIFE_DX, LIFE_DY, 0.0f);
	g_Life.w = LIFE_DW;
	g_Life.h = LIFE_DH;
	for (int i = 0; i < g_Life.life_max; i++)
	{
		//�����郉�C�t���͐Ԃ̃n�[�g���A���݃��C�t�l�ƃ��C�t�ő�l�̍����ɂ͍��̃n�[�g��`�悷��
		if (g_Life.life > i)
		{
			g_Life.texNo = HART;
		}
		else
		{
			g_Life.texNo = HART_LOST;
		}

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Life.texNo]);

		//�n�[�g�̍��E�����ǂ����`�悷�邩�v�Z
		int number = i % 2;

		// ���C�t�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Life.pos.x + ((g_Life.w / 2)*i);	// ���C�t�̕\���ʒuX
		float py = g_Life.pos.y;				// ���C�t�̕\���ʒuY
		float pw = g_Life.w / 2;					// ���C�t�̕\����
		float ph = g_Life.h;					// ���C�t�̕\������

		float tw = 1.0f / LIFE_DIGIT;					// �e�N�X�`���̕�
		float th = 1.0f;					// �e�N�X�`���̍���
		float tx = tw * (float)(number);						// �e�N�X�`���̍���X���W
		float ty = 0.0f;						// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	g_Life.texNo = HART_BOS;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Life.texNo]);
	g_Life.w = LIFE_BW;
	g_Life.h = LIFE_BH;
	g_Life.pos = XMFLOAT3(LIFE_DX - (LIFE_BW * 0.7f), LIFE_DY + (LIFE_BH * 0.2f), 0.0f);	//�����̔������̌��A���W������
	// ���C�t�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_Life.pos.x;				// ���C�t�̕\���ʒuX
	float py = g_Life.pos.y;				// ���C�t�̕\���ʒuY
	float pw = g_Life.w;					// ���C�t�̕\����
	float ph = g_Life.h;					// ���C�t�̕\������

	float tw = 1.0f;						// �e�N�X�`���̕�
	float th = 1.0f;						// �e�N�X�`���̍���
	float tx = 0.0f;						// �e�N�X�`���̍���X���W
	float ty = 0.0f;						// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}
//=============================================================================
// �Q�[���I�[�o�[�`�揈��
//=============================================================================
void DrawGameOver(void)
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
	g_Life.texNo = GAMEOVER;
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Life.texNo]);

	// �Q�[���I�[�o�[�����̈ʒu��e�N�X�`���[���W�𔽉f
	float px = SCREEN_WIDTH * 0.5f;		// �Q�[���I�[�o�[�����̕\���ʒuX
	float py = SCREEN_HEIGHT * 0.5f;	// �Q�[���I�[�o�[�����̕\���ʒuY
	float pw = GAMEOVER_DW;				// �Q�[���I�[�o�[�����̕\����
	float ph = GAMEOVER_DH;				// �Q�[���I�[�o�[�����̕\������

	float tw = 1.0f;					// �e�N�X�`���̕�
	float th = 1.0f;					// �e�N�X�`���̍���
	float tx = 0.0f;					// �e�N�X�`���̍���X���W
	float ty = 0.0f;					// �e�N�X�`���̍���Y���W
	//�t�F�[�h�C���p����
	if (lifeAlpha < 1.0f)				
		lifeAlpha += ALPHA_COUNT;
	float alpha = lifeAlpha;
	//�t�F�[�h�C�����RGN�l��ύX���Ă���
	if (lifeAlpha >= 1.0f && lifeGB < 1.0f)	
		lifeGB += ALPHA_COUNT;
	float minus = lifeGB;
	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f - minus, 1.0f - minus, alpha));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}

//=============================================================================
// ���C�t���v�Z����
// ����:add :���C�t�̕ω���	chara: ���C�t�𑝌�������Ώۂ𔻕� i:�\���̔z��̉��ԖځH
// �߂�l:true�Ȃ琶���Afalse�Ȃ玀�S��ԂƂ���player.cpp�ɕԂ�
//=============================================================================
void AddLife(int add, int chara, int i)//���Z�ʁA�Ώ�(0�Ȃ�v���C���[�A1�Ȃ�G�l�~�[)�A�z��ԍ�
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	CLONE *clone = GetClone();
	if (GetPlayMode() == TUTORIAL_GAME)return;	//�`���[�g���A���͉��Z����

	switch (chara)			//0�Ȃ�v���C���[�̃��C�t�𑝌�������Bg_Life�Ƀv���C���[�̃��C�t�����i�[
	{
	case 0:
		g_Life.life = player->life;
		g_Life.life_max = player->lifeMax;
		g_Life.life += add;
		if (g_Life.life > g_Life.life_max)
		{
			g_Life.life = g_Life.life_max;
			AddScore(1000);					//�ő�HP�𒴂���񕜂����Ă���Ȃ�X�R�A�����Z������
		}
		player->life = g_Life.life;
		break;

	case 1:	//1�Ȃ�G�l�~�[�̃��C�t�𑝌�������Bg_Life�ɃG�l�~�[�̃��C�t�����i�[

		g_Life.life = enemy[i].life;
		g_Life.life_max = enemy[i].lifeMax;
		g_Life.life += add;
		if (g_Life.life > g_Life.life_max)
		{
			g_Life.life = g_Life.life_max;
		}
		enemy[i].life = g_Life.life;
		break;

	case 2:	//2�Ȃ�N���[���̃��C�t�𑝌�������

		g_Life.life = clone[i].life;
		g_Life.life_max = clone[i].lifeMax;
		g_Life.life += add;
		if (g_Life.life > g_Life.life_max)
		{
			g_Life.life = g_Life.life_max;
		}
		clone[i].life = g_Life.life;
		break;

	}
}


int GetLife(void)
{
	return g_Life.life;
}


void SetLife(int life)
{
	g_Life.life = life;
}



