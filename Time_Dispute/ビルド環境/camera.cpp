//=============================================================================
//
// �J�������� [camera.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "game.h"
#include "player.h"
#include "time.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// �r���[���ʂ̎���p
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��	
#define	VIEW_NEAR_Z		(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z		(10000.0f)										// �r���[���ʂ�FarZ�l

#define	VALUE_MOVE_CAMERA	(2.0f)										// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)								// �J�����̉�]��

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CAMERA			g_Camera;		// �J�����f�[�^

static int				g_ViewPortType = TYPE_FULL_SCREEN;

//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	g_Camera.pos = { GAME_X_CAM, GAME_Y_CAM, GAME_Z_CAM };
	g_Camera.at = { 0.0f, 0.0f, 0.0f };
	g_Camera.atPos = { 0.0f, 0.0f, 0.0f };
	g_Camera.up  = { 0.0f, 1.0f, 0.0f };
	g_Camera.rot = { 0.0f, 0.0f, 0.0f };

	// ���_�ƒ����_�̋������v�Z
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	g_Camera.len = sqrtf(vx * vx + vz * vz);
	
	g_Camera.tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Camera.move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
	g_Camera.tbl_size = NULL;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// �r���[�|�[�g�^�C�v�̏�����
	g_ViewPortType = TYPE_FULL_SCREEN;
}


//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{
	PLAYER *player = GetPlayer();

	if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, DIK_R) && (GetTime() > 0 && player->life > 0))
	{// ���_����u���v
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, DIK_L) && (GetTime() > 0 && player->life > 0))
	{// ���_����u�E�v
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}


#ifdef _DEBUG

	if (GetKeyboardPress(DIK_Y))
	{// ���_�ړ��u��v
		g_Camera.pos.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_N))
	{// ���_�ړ��u���v
		g_Camera.pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_Q))
	{// �����_����u���v
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_E))
	{// �����_����u�E�v
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_T))
	{// �����_�ړ��u��v
		g_Camera.at.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_B))
	{// �����_�ړ��u���v
		g_Camera.at.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_U))
	{// �߂Â�
		g_Camera.len -= VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// �����
		g_Camera.len += VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	// �J�����������ɖ߂�
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}

#endif
	if (g_Camera.tbl_adr != NULL)
	{
		// �ړ�����
		int		index = (int)g_Camera.move_time;
		float	time = g_Camera.move_time - index;
		int		size = g_Camera.tbl_size;

		float dt = 1.0f / g_Camera.tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
		g_Camera.move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

		if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂��B���`��ԃf�[�^������
		{
			g_Camera.move_time = 0.0f;
			index = 0;
			g_Camera.tbl_adr = NULL;
			return;
		}

		// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
		XMVECTOR p1 = XMLoadFloat3(&g_Camera.tbl_adr[index + 1].pos);	// ���̏ꏊ
		XMVECTOR p0 = XMLoadFloat3(&g_Camera.tbl_adr[index + 0].pos);	// ���݂̏ꏊ
		XMVECTOR vec = p1 - p0;
		XMStoreFloat3(&g_Camera.pos, p0 + vec * time);

		// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
		XMVECTOR r1 = XMLoadFloat3(&g_Camera.tbl_adr[index + 1].rot);	// ���̊p�x
		XMVECTOR r0 = XMLoadFloat3(&g_Camera.tbl_adr[index + 0].rot);	// ���݂̊p�x
		XMVECTOR rot = r1 - r0;
		XMStoreFloat3(&g_Camera.rot, r0 + rot * time);

		// �����_�̕ύX�ʂ����߂� S = StartX + (EndX - StartX) * ���̎���
		XMVECTOR s1 = XMLoadFloat3(&g_Camera.tbl_adr[index + 1].scl);	// ���̒����_
		XMVECTOR s0 = XMLoadFloat3(&g_Camera.tbl_adr[index + 0].scl);	// ���݂̒����_
		XMVECTOR scl = s1 - s0;
		XMStoreFloat3(&g_Camera.atPos, s0 + scl * time);

	}


#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}


//=============================================================================
// �J�����̍X�V
//=============================================================================
void SetCamera(void) 
{
	// �r���[�}�g���b�N�X�ݒ�
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);


	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


//=============================================================================
// �J�����̎擾
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &g_Camera;
}

int GetViewPortType(void)
{
	return g_ViewPortType;
}



// �J�����̎��_�ƒ����_���Z�b�g
void SetCameraAT(XMFLOAT3 pos)
{
	//�����̍��W�ɕύX�ʂ����Z�B
	pos.x += g_Camera.atPos.x;
	pos.y += g_Camera.atPos.y;
	pos.z += g_Camera.atPos.z;
	// �J�����̒����_���Z�b�g
	g_Camera.at = pos;

	// �J�����̎��_���J������Y����]�ɑΉ������Ă���
	g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
	g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;

}

