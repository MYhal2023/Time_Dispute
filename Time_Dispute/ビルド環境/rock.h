//=============================================================================
//
// �⃂�f������ [rock.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ROCK			"data/MODEL/iwa.obj"			// �ǂݍ��ރ��f����
#define MAX_ROCK		(6)					// ��̐�

#define	ROCK_SIZE		(30.0f)				// �����蔻��̑傫���̔��a
#define	ROCK01_X_AI		(20)				// AI�Ɉ����n����̍��W���
#define	ROCK01_Z_AI		(30)				// 
#define ROCK01_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK01_X_AI * BLOCK_SIZE)
#define ROCK01_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK01_Z_AI * BLOCK_SIZE)

#define	ROCK02_X_AI		(80)				// AI�Ɉ����n����̍��W���
#define	ROCK02_Z_AI		(60)				// 
#define ROCK02_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK02_X_AI * BLOCK_SIZE)
#define ROCK02_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK02_Z_AI * BLOCK_SIZE)

#define	ROCK03_X_AI		(100)				// AI�Ɉ����n����̍��W���
#define	ROCK03_Z_AI		(90)				// 
#define ROCK03_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK03_X_AI * BLOCK_SIZE)
#define ROCK03_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK03_Z_AI * BLOCK_SIZE)

#define	ROCK04_X_AI		(30)				// AI�Ɉ����n����̍��W���
#define	ROCK04_Z_AI		(80)				// 
#define ROCK04_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK04_X_AI * BLOCK_SIZE)
#define ROCK04_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK04_Z_AI * BLOCK_SIZE)

#define	ROCK05_X_AI		(50)				// AI�Ɉ����n����̍��W���
#define	ROCK05_Z_AI		(20)				// 
#define ROCK05_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK05_X_AI * BLOCK_SIZE)
#define ROCK05_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK05_Z_AI * BLOCK_SIZE)

#define	ROCK06_X_AI		(90)				// AI�Ɉ����n����̍��W���
#define	ROCK06_Z_AI		(30)				// 
#define ROCK06_POS_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + ROCK06_X_AI * BLOCK_SIZE)
#define ROCK06_POS_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - ROCK06_Z_AI * BLOCK_SIZE)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ROCK
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float				size;				// �����蔻��̑傫��
	
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitRock(void);
void UninitRock(void);
void UpdateRock(void);
void DrawRock(void);

ROCK *GetRock(void);
BOOL CheckHitBattle(int i);