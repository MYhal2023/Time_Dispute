//=============================================================================
//
// �J�������� [camera.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	GAME_X_CAM			(0.0f)			// �J�����̏����ʒu(X���W)
#define	GAME_Y_CAM			(50.0f)			// �J�����̏����ʒu(Y���W)
#define	GAME_Z_CAM			(-140.0f)		// �J�����̏����ʒu(Z���W)




enum {
	TYPE_FULL_SCREEN,
	TYPE_LEFT_HALF_SCREEN,
	TYPE_RIGHT_HALF_SCREEN,
	TYPE_UP_HALF_SCREEN,
	TYPE_DOWN_HALF_SCREEN,
	TYPE_LIGHT_SCREEN,
	TYPE_NONE,

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

CAMERA *GetCamera(void);

int GetViewPortType(void);

void SetCameraAT(XMFLOAT3 pos);
