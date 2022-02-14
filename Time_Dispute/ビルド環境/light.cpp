//=============================================================================
//
// ���C�g���� [light.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "player.h"
#include "time.h"
#include "light.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
#define LIGHT_ANGLE_X	(0.5f)		//�A���O���̌X���x����
#define LIGHT_ANGLE_Z	(0.5f)		//�A���O���̌X���x����
#define CHANGE_ANGLE	(0.0005f)		//�A���O���̌X���x����

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;
//=============================================================================
// ����������
//=============================================================================
void InitLight(void)
{

	//���C�g������
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position = XMFLOAT3(100.0f, 300.0f, 100.0f);
		g_Light[i].Direction = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Light[i].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Light[i].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		g_Light[i].Attenuation = 100.0f;	// ��������
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ���C�g�̃^�C�v
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// ���s�����̐ݒ�i���E���Ƃ炷���j
	g_Light[0].Position = XMFLOAT3(0.0f, 0.0f, 00.0f);
	g_Light[0].Direction = XMFLOAT3(LIGHT_ANGLE_X, -1.0f, LIGHT_ANGLE_Z);
	g_Light[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// ���s����
	g_Light[0].Enable = TRUE;									// ���̃��C�g��ON
	SetLight(0, &g_Light[0]);									// ����Őݒ肵�Ă���



	// �t�H�O�̏������i���̌��ʁj
	g_Fog.FogStart = 200.0f;									// ���_���炱�̋��������ƃt�H�O��������n�߂�
	g_Fog.FogEnd   = 600.0f;									// �����܂ŗ����ƃt�H�O�̐F�Ō����Ȃ��Ȃ�
	g_Fog.FogColor = XMFLOAT4( 0.9f, 0.9f, 0.9f, 1.0f );		// �t�H�O�̐F
	SetFog(&g_Fog);
	SetFogEnable(TRUE);		// ���̏ꏊ���`�F�b�N���� shadow
}




//=============================================================================
// ���C�g�̐ݒ�
// Type�ɂ���ăZ�b�g���郁���o�[�ϐ����ς���Ă���
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// �t�H�O�̐ݒ�
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}

//���C�g�̒���
void UpdateLight(void)
{
	PLAYER *player = GetPlayer();
	g_Light[0].Position = XMFLOAT3(player->pos.x, 500.0f, player->pos.z - 400.0f);
	g_Light[0].Direction = XMFLOAT3(LIGHT_ANGLE_X + (player->pos.x * CHANGE_ANGLE), -1.0f, LIGHT_ANGLE_Z + (player->pos.z * CHANGE_ANGLE));
	SetLight(0, &g_Light[0]);									// ����Őݒ肵�Ă���
}