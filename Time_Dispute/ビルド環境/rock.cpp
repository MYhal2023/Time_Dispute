//=============================================================================
//
// �⃂�f������ [rock.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "rock.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "collision.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	VALUE_MOVE			(1.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define ROCK_OFFSET_Y		(0.0f)							// ��̑��������킹��
#define ROCK_ATK_INTERVAL	(1.0f)							// ��̍U���Ԋu
#define ROCK_INTERVAL		(240.0f)						// ��̐����Ԋu


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ROCK			g_Rock[MAX_ROCK];				// ��

static BOOL			g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitRock(void)
{
	for (int i = 0; i < MAX_ROCK; i++)
	{
		g_Rock[i].load = TRUE;

		g_Rock[i].pos = XMFLOAT3(0.0f, ROCK_OFFSET_Y, 0.0f);
		if(i == 0)
			g_Rock[i].pos = XMFLOAT3(ROCK01_POS_X, ROCK_OFFSET_Y, ROCK01_POS_Z);
		else if(i == 1)
			g_Rock[i].pos = XMFLOAT3(ROCK02_POS_X, ROCK_OFFSET_Y, ROCK02_POS_Z);
		else if(i == 2)
			g_Rock[i].pos = XMFLOAT3(ROCK03_POS_X, ROCK_OFFSET_Y, ROCK03_POS_Z);
		else if (i == 3)
			g_Rock[i].pos = XMFLOAT3(ROCK04_POS_X, ROCK_OFFSET_Y, ROCK04_POS_Z);
		else if (i == 4)
			g_Rock[i].pos = XMFLOAT3(ROCK05_POS_X, ROCK_OFFSET_Y, ROCK05_POS_Z);
		else if (i == 5)
			g_Rock[i].pos = XMFLOAT3(ROCK06_POS_X, ROCK_OFFSET_Y, ROCK06_POS_Z);

		g_Rock[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Rock[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Rock[i].size = ROCK_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Rock[0].model, &g_Rock[0].diffuse[0]);

		g_Rock[i].use = TRUE;			// 

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitRock(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].load)
		{
			UnloadModel(&g_Rock[i].model);
			g_Rock[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateRock(void)
{
	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].use != TRUE)		// ���̊₪�g���Ă��Ȃ��Ȃ珈�����X�L�b�v
			continue;
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawRock(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);
	// Z��r�Ȃ�
	SetDepthEnable(TRUE);

	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Rock[i].scl.x, g_Rock[i].scl.y, g_Rock[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Rock[i].rot.x, g_Rock[i].rot.y + XM_PI, g_Rock[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Rock[i].pos.x, g_Rock[i].pos.y, g_Rock[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Rock[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Rock[i].model);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
	// Z��r�Ȃ�
	SetDepthEnable(TRUE);

}

//=============================================================================
// ��̎擾
//=============================================================================
ROCK *GetRock(void)
{
	return &g_Rock[0];
}
