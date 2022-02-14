//=============================================================================
//
// �N���[�����f������ [clone.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "math.h"
#include "renderer.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "collision.h"
#include "player.h"
#include "coin.h"
#include "extension.h"
#include "debugproc.h"
#include "rock.h"
#include "clone.h"
#include "salieri.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	CLONE_VALUE_MOVE			(1.0f)					// �ړ���
#define	CLONE_VALUE_AT_MOVE		(2.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define CLONE_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define CLONE_OFFSET_Y		(0.0f)							// �G�l�~�[�̑��������킹��
#define CLONE_EYESIGHT		(200.0f)						// �G�l�~�[�̎���

#define CLONE_TPS_INTERVAL	(CLONE_POINT_INTERVAL)			//��̓C���^�[�o��
#define CLONE_AT_INTERVAL	(60.0f)
#define CLONE_INVINC_FLAME	(120.0f)						// �v���C���[���G�t���[��
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CLONE			g_Clone[MAX_CLONE];				// �G�l�~�[
static BOOL				g_Load = FALSE;
static float			interval[MAX_CLONE];			//TPS���Q�Ƃ���Ԋu
static float			frameCount[MAX_CLONE];
static int				atCount;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitClone(void)
{
	for (int i = 0; i < MAX_CLONE; i++)
	{
		g_Clone[i].load = TRUE;

		g_Clone[i].pos = XMFLOAT3(0.0f, CLONE_OFFSET_Y, 0.0f);
		g_Clone[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Clone[i].nextrot = g_Clone[i].rot;
		g_Clone[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Clone[i].moveVec = { 0.0f, 0.0f, 0.0f };
		g_Clone[i].frameRot = 0.0f;
		g_Clone[i].size = CLONE_SIZE;	// �����蔻��̑傫��
		g_Clone[i].spd = 0.0f;
		g_Clone[i].life = 1;
		g_Clone[i].lifeMax = g_Clone[i].life;
		g_Clone[i].character = CLONE_OFFENSIVE;
		g_Clone[i].use = FALSE;			// TRUE:�����Ă� �������i�K�ł͖��g�p��

		g_Clone[i].attack = FALSE;
		g_Clone[i].attackUse = FALSE;

		g_Clone[i].atInvinc = FALSE;				//���G���Ԓ����ۂ�
		g_Clone[i].atInvincTime = 0.0f;			//���Ԍv���p
		g_Clone[i].blink = 0.0f;
		g_Clone[i].blinking = 10.0f;
		g_Clone[i].blinkingMax = 20.0f;
		g_Clone[i].blinkingCount = 2.0f;

		interval[i] = 0.0f;				//TPS���Q�Ƃ���Ԋu
		frameCount[i] = 0.0f;
	}
	atCount = 0;

	g_Load = TRUE;						//�ǂݍ��݊���
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitClone(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_CLONE; i++)
	{
		if (g_Clone[i].load)
		{
			UnloadModel(&g_Clone[i].model);
			g_Clone[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateClone(void)
{
	for (int i = 0; i < MAX_CLONE; i++)
	{
		if (g_Clone[i].use != TRUE)		// ���̃G�l�~�[���g���Ă��Ȃ��Ȃ珈�����X�L�b�v
			continue;

		if (g_Clone[i].life <= 0)//���C�t��0�Ȃ�����Ď��̃N���[�������
		{
			g_Clone[i].use = FALSE;
			continue;
		}

		float old_x = g_Clone[i].pos.x;
		float old_z = g_Clone[i].pos.z;
		//�C���^�[�o���J�E���g�����ȏォ��U�����łȂ���΁ATPS�����ɒP�ʃx�N�g�����X�V
		interval[i] += 1.0f;
		if (interval[i] >= CLONE_TPS_INTERVAL && g_Clone[i].attack == FALSE)
		{
			g_Clone[i].moveVec = CloneTacticalPointSystem(i);
			interval[i] = 0.0f;
			frameCount[i] = 0.0f;
		}
		//���x��ݒ�
		g_Clone[i].spd = CLONE_VALUE_MOVE;

		//�U�����Ȃ�H
		if (g_Clone[i].attack == TRUE)
		{
			atCount++;
			g_Clone[i].spd = CLONE_VALUE_AT_MOVE;	//���x���㏑��
			g_Clone[i].rot.x = XM_PI * 0.25f;
			if (atCount >= CLONE_AT_INTERVAL)	//�U���t���[�����I��������U�������Z�b�g
			{
				g_Clone[i].attack = FALSE;
				g_Clone[i].attackUse = FALSE;
				g_Clone[i].rot.x = 0.0f;
				atCount = 0;
			}
		}

		XMVECTOR now = XMLoadFloat3(&g_Clone[i].pos);								// ���݂̏ꏊ
		XMStoreFloat3(&g_Clone[i].pos, now + g_Clone[i].moveVec * g_Clone[i].spd);	//�P�ʃx�N�g�������Ɉړ�

		if (frameCount[i] < TPS_ROT_CLONE_SPEED)
		{
			g_Clone[i].rot.y += g_Clone[i].frameRot;
			frameCount[i] += 1.0f;
		}
		else if (frameCount[i] >= TPS_ROT_CLONE_SPEED)
		{
			g_Clone[i].rot.y = g_Clone[i].nextrot.y;
		}


		for (int i = 0; i < MAX_ROCK; i++)
		{
			ROCK *rock = GetRock();
			XMFLOAT3 pos = g_Clone[i].pos;
			pos.y = 0.0f;
			if (CollisionBC(pos, rock[i].pos, g_Clone[i].size, rock[i].size) == TRUE)
			{
				g_Clone[i].pos.x = old_x;
				g_Clone[i].pos.z = old_z;
				break;
			}
		}

		//���b�V���t�B�[���h�͈͊O�ɏo�Ȃ��悤�ɂ���
		if (g_Clone[i].pos.x <= CLONE_MIN_X ||
			g_Clone[i].pos.x >= CLONE_MAX_X ||
			g_Clone[i].pos.z <= CLONE_MIN_Z ||
			g_Clone[i].pos.z >= CLONE_MAX_Z)
		{
			g_Clone[i].pos.x = old_x;
			g_Clone[i].pos.z = old_z;
		}

		// ���C�L���X�g���đ����̍��������߂�
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
		XMFLOAT3 hitPosition;								// ��_
		hitPosition.y = g_Clone[i].pos.y - CLONE_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
		bool ans = RayHitField(g_Clone[i].pos, &hitPosition, &normal);
		g_Clone[i].pos.y = hitPosition.y + CLONE_OFFSET_Y;


		if (g_Clone[i].atInvinc == TRUE)	//��_���[�W�ɂ�閳�G���ɂ��邱�Ƃ́H
		{
			CloneIncibleEffect(i);
			g_Clone[i].atInvincTime += 1.0f;
			if (g_Clone[i].atInvincTime >= CLONE_INVINC_FLAME)//���G���Ԃ��I��������
			{
				g_Clone[i].blinkingCount = 1.0f;
				g_Clone[i].blink = 0.0f;
				g_Clone[i].atInvincTime = 0.0f;
				g_Clone[i].atInvinc = FALSE;
			}
		}

	}
#ifdef _DEBUG	// �f�o�b�O����\������
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawClone(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_CLONE; i++)
	{
		if (g_Clone[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Clone[i].scl.x, g_Clone[i].scl.y, g_Clone[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Clone[i].rot.x, g_Clone[i].rot.y, g_Clone[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Clone[i].pos.x, g_Clone[i].pos.y, g_Clone[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Clone[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Clone[i].model);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
CLONE *GetClone(void)
{
	return &g_Clone[0];
}


void SetClone(XMFLOAT3 pos , int character)
{
	// ��������
	int clone_count = 0;
	for (int i = 0; i < MAX_CLONE; i++)
	{
		if (g_Clone[i].use == FALSE)
			continue;

		clone_count++;
	}
	if (clone_count >= MAX_CLONE)//�ő吔����Ȃ牽�����Ȃ�
		return;

	for (int i = 0; i < MAX_CLONE; i++)	//���g�p�̔z��ԍ��ɃA�N�Z�X
	{
		if (g_Clone[i].use != FALSE)	//�g�p�ς݂Ȃ�X�L�b�v
			continue;

		g_Clone[i].pos = { pos.x, pos.y, pos.z };
		g_Clone[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Clone[i].nextrot = g_Clone[i].rot;
		g_Clone[i].scl = { 0.64f, 0.8f, 0.8f };
		g_Clone[i].use = TRUE;
		g_Clone[i].moveVec = { 0.0f, 0.0f, 0.0f };
		g_Clone[i].frameRot = 0.0f;
		g_Clone[i].size = CLONE_SIZE;			// �����蔻��̑傫��
		g_Clone[i].spd = 0.0f;
		g_Clone[i].life = 1;
		g_Clone[i].lifeMax = g_Clone[i].life;
		g_Clone[i].character = character;

		g_Clone[i].attack = FALSE;
		g_Clone[i].attackUse = FALSE;

		g_Clone[i].atInvinc = FALSE;			//���G���Ԓ����ۂ�
		g_Clone[i].atInvincTime = 0.0f;			//���Ԍv���p
		g_Clone[i].blink = 0.0f;
		g_Clone[i].blinking = 10.0f;
		g_Clone[i].blinkingMax = 20.0f;
		g_Clone[i].blinkingCount = 2.0f;

		CloneChangeDiffuse(i);
		switch(g_Clone[i].character)//�L�����N�^�[�ʂɃX�e�[�^�X����
		{
		case CLONE_OFFENSIVE:
			g_Clone[i].life = 2;
			g_Clone[i].lifeMax = g_Clone[i].life;
			break;
		case CLONE_ITEM:
			g_Clone[i].life = 4;
			g_Clone[i].lifeMax = g_Clone[i].life;
			break;
		case CLONE_ALL_TAKE:
			g_Clone[i].life = 7;
			g_Clone[i].lifeMax = g_Clone[i].life;
			break;
		}
		interval[i] = 0.0f;						//TPS���Q�Ƃ���Ԋu
		frameCount[i] = 0.0f;
		break;
	}
}



void CloneChangeDiffuse(int i)
{
	//���i���ƂɐF���㏑��
	if (g_Clone[i].character == CLONE_OFFENSIVE)
	{
		SetModelDiffuse(&g_Clone[i].model, 0, { 1.0f, 0.0f, 0.0f ,1.0f });
	}
	else if (g_Clone[i].character == CLONE_ITEM)
	{
		SetModelDiffuse(&g_Clone[i].model, 0, { 0.5f, 0.5f, 0.5f ,1.0f });
	}
	else if (g_Clone[i].character == CLONE_ALL_TAKE)
	{
		SetModelDiffuse(&g_Clone[i].model, 0, { 1.0f, 1.0f, 0.0f ,1.0f });
	}

}

void CloneIncibleEffect(int i)
{
	if (g_Clone[i].blinking < 0)
	{
		g_Clone[i].blinkingCount *= -1;
		g_Clone[i].blink -= 1.0f;
	}

	if (g_Clone[i].blinking > g_Clone[i].blinkingMax)
	{
		g_Clone[i].blinkingCount *= -1;
		g_Clone[i].blink += 1.0f;
	}
	g_Clone[i].blinking += g_Clone[i].blinkingCount;
}
