//=============================================================================
//
// ���f������ [player.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "game.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "rock.h"
#include "collision.h"
#include "time.h"
#include "amadeus.h"
#include "clone.h"
#include "money.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	VALUE_MOVE			(2.0f)							// �ړ���
#define	VALUE_AT_MOVE		(4.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(1.0f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(0.0f)							// �v���C���[�̑��������킹��
#define PLAYER_OFFSET_Z		(-300.0f)							// �v���C���[�̑��������킹��
#define PLAYER_LIFE			(4)								// �v���C���[�̃��C�t

#define PLAYER_PARTS_MAX	(1)								// �v���C���[�̃p�[�c�̐�
#define PLAYER_AT_FLAME		(30.0f)							// �v���C���[�̍U���t���[��
#define PLAYER_INVINC_FLAME	(120.0f)						// �v���C���[���G�t���[��
#define DEFER				(2.0f)							// �v���C���[�ړ������̗]�蕝
#define PLAYER_MIN_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + DEFER * BLOCK_SIZE)				// �v���C���[�ړ����
#define PLAYER_MIN_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - (FIELD_Z - DEFER) * BLOCK_SIZE)	// 
#define PLAYER_MAX_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + (FIELD_X - DEFER) * BLOCK_SIZE )	// 
#define PLAYER_MAX_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - DEFER * BLOCK_SIZE)				// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[

static BOOL			g_Load = FALSE;
static int			atCount;


// �v���C���[�̊K�w�A�j���[�V�����f�[�^
static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		 XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Player.load = TRUE;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, PLAYER_OFFSET_Z };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 0.8f, 1.0f, 1.0f };
	g_Player.moveVec = { 0.0f, 0.0f, 0.0f };
	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��
	g_Player.life = PLAYER_LIFE;
#ifdef DEBUG
	g_Player.life = 10;
#endif
	g_Player.lifeMax = g_Player.life;
	g_Player.use = TRUE;
	g_Player.attack = FALSE;
	g_Player.attackUse = FALSE;

	g_Player.atInvinc = FALSE;				//���G���Ԓ����ۂ�
	g_Player.atInvincTime = 0.0f;			//���Ԍv���p
	g_Player.blink = 0.0f;
	g_Player.blinking = 10.0f;
	g_Player.blinkingMax = 20.0f;
	g_Player.blinkingCount = 2.0f;


	// �K�w�A�j���[�V�����p�̏���������
	g_Player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

	g_Load = TRUE;
	atCount = 0;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// ���f���̉������
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	SettingPlayer();	//���C�L���X�g�ƃN�H�[�^�j�I������

	// ���f���̐F��ύX
	for (int i = 0; i < g_Player.model.SubsetNum; i++)
	{
		SetModelDiffuse(&g_Player.model, i, XMFLOAT4(1.0f, 1.0f - g_Player.blink, 1.0f - g_Player.blink, 1.0f));
	}

	//��������v���C���[����Ɋւ��鏈��
	if (GetPlayMode() <= TUTORIAL)
		return;

	//�v���C���[�̋����W��ۑ�
	float old_x = g_Player.pos.x;
	float old_z = g_Player.pos.z;

	CAMERA *cam = GetCamera();
	if (g_Player.attack != TRUE && GetTime() > 0 && g_Player.life > 0)	//�U�������Q�[���I�[�o�[�łȂ���Έړ��\
	{
		// �ړ��{�^��
		if (IsButtonPressed(0, BUTTON_LEFT) || GetKeyboardPress(DIK_A))
		{	// ���ֈړ�
			g_Player.spd = VALUE_MOVE;
			g_Player.moveVec.x -= cosf(cam->rot.y) * 0.025f;
			g_Player.moveVec.z += sinf(cam->rot.y) * 0.025f;
		}
		if (IsButtonPressed(0, BUTTON_RIGHT) || GetKeyboardPress(DIK_D))
		{	// �E�ֈړ�
			g_Player.spd = VALUE_MOVE;
			g_Player.moveVec.x += cosf(cam->rot.y) * 0.025f;
			g_Player.moveVec.z -= sinf(cam->rot.y) * 0.025f;
		}
		if (IsButtonPressed(0, BUTTON_UP) || GetKeyboardPress(DIK_W))
		{	// ��ֈړ�
			g_Player.spd = VALUE_MOVE;
			g_Player.moveVec.x += sinf(cam->rot.y) * 0.025f;
			g_Player.moveVec.z += cosf(cam->rot.y) * 0.025f;
		}
		if (IsButtonPressed(0, BUTTON_DOWN) || GetKeyboardPress(DIK_S))
		{	// ���ֈړ�
			g_Player.spd = VALUE_MOVE;
			g_Player.moveVec.x -= sinf(cam->rot.y) * 0.025f;
			g_Player.moveVec.z -= cosf(cam->rot.y) * 0.025f;
		}
	}

	//�U���{�^��
	if ((GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A) || GetKeyboardTrigger(DIK_SPACE)) && GetTime() > 0 && g_Player.life > 0)
	{
		g_Player.attack = TRUE;
		g_Player.attackUse = TRUE;
	}

	if (((GetKeyboardTrigger(DIK_1) || IsButtonTriggered(0, BUTTON_B)) && GetMoney() >= CLONE_OFFENSIVE_VALUE) && GetTime() > 0 && g_Player.life > 0)
	{
		SetClone(g_Player.pos, CLONE_OFFENSIVE);
		AddMoney(-CLONE_OFFENSIVE_VALUE);
	}
	else if (((GetKeyboardTrigger(DIK_2) || IsButtonTriggered(0, BUTTON_X)) && GetMoney() >= CLONE_TAKE_TIME_VALUE) && GetTime() > 0 && g_Player.life > 0)
	{
		SetClone(g_Player.pos, CLONE_ITEM);
		AddMoney(-CLONE_TAKE_TIME_VALUE);
	}
	else if (((GetKeyboardTrigger(DIK_3) || IsButtonTriggered(0, BUTTON_Y)) && GetMoney() >= CLONE_CROW_VALUE) && GetTime() > 0 && g_Player.life > 0)
	{
		SetClone(g_Player.pos, CLONE_ALL_TAKE);
		AddMoney(-CLONE_CROW_VALUE);
	}

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}
#endif

	if (g_Player.attack == TRUE)
	{
		atCount++;
		g_Player.spd = VALUE_AT_MOVE;
		g_Player.rot.x = XM_PI * 0.25f;
		if (atCount >= PLAYER_AT_FLAME)	//�U���t���[�����I��������U�������Z�b�g
		{
			g_Player.attack = FALSE;
			g_Player.attackUse = FALSE;
			g_Player.rot.x = 0.0f;
			atCount = 0;
		}

	}

	// Key���͂���������ړ���������
	if (g_Player.spd > 0.0f)
	{
		float angle = atan2f(g_Player.moveVec.x, g_Player.moveVec.z);
		g_Player.rot.y = angle;
	}
	//g_Player.moveVec.x = sinf(g_Player.rot.y);
	//g_Player.moveVec.z = cosf(g_Player.rot.y);
	//�ړ��l���x�N�g���ϊ����Ĉړ�������
	XMVECTOR moveVec = XMLoadFloat3(&g_Player.moveVec);
	XMVECTOR now = XMLoadFloat3(&g_Player.pos);								// ���݂̏ꏊ
	XMStoreFloat3(&g_Player.pos, now + XMVector3Normalize(moveVec) * g_Player.spd);	//�P�ʃx�N�g�������Ɉړ�


	ROCK *rock = GetRock();
	for (int i = 0; i < MAX_ROCK; i++)
	{
		XMFLOAT3 pos = g_Player.pos;
		pos.y = 0.0f;
		if (CollisionBC(pos, rock[i].pos, g_Player.size, rock[i].size) == TRUE)
		{
			g_Player.pos.x = old_x;
			g_Player.pos.z = old_z;
			break;
		}
	}

	//���b�V���t�B�[���h�͈͊O�ɏo�Ȃ��悤�ɂ���
	if (g_Player.pos.x <= PLAYER_MIN_X ||
		g_Player.pos.x >= PLAYER_MAX_X ||
		g_Player.pos.z <= PLAYER_MIN_Z ||
		g_Player.pos.z >= PLAYER_MAX_Z)
	{
		g_Player.pos.x = old_x;
		g_Player.pos.z = old_z;
	}

	if (g_Player.atInvinc == TRUE)	//��_���[�W�ɂ�閳�G���ɂ��邱�Ƃ́H
	{
		IncibleEffect();
		g_Player.atInvincTime += 1.0f;
		if (g_Player.atInvincTime >= PLAYER_INVINC_FLAME)//���G���Ԃ��I��������
		{
			g_Player.blinkingCount = 1.0f;
			g_Player.blink = 0.0f;
			g_Player.atInvincTime = 0.0f;
			g_Player.atInvinc = FALSE;
		}
	}


	g_Player.spd *= 0.8f;
	g_Player.moveVec.x *= 0.8f;
	g_Player.moveVec.z *= 0.8f;



#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Player:�� �� �� ���@Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//// �N�H�[�^�j�I���𔽉f
	XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	
	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	SetFuchi(1);

	// ���f���`��
	DrawModel(&g_Player.model);


	SetFuchi(0);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

void SettingPlayer(void)
{
	// ���C�L���X�g���đ����̍��������߂�
	XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
	XMFLOAT3 hitPosition;								// ��_
	hitPosition.y = g_Player.pos.y - PLAYER_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
	bool ans = RayHitField(g_Player.pos, &hitPosition, &normal);
	g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;
	//g_Player.pos.y = PLAYER_OFFSET_Y;



	g_Player.spd *= 0.5f;


//////////////////////////////////////////////////////////////////////
// �p������
//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;

	// �Q�̃x�N�g���̊O�ς�����ĔC�ӂ̉�]�������߂�
	g_Player.upVector = normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.upVector));	//���ꂪ�C�ӂ̎�

	// ���߂���]������N�H�[�^�j�I�������o��
	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	angle = asinf(len);
	quat = XMQuaternionRotationNormal(nvx, angle);

	// �O��̃N�H�[�^�j�I�����獡��̃N�H�[�^�j�I���܂ł̉�]�����炩�ɂ���
	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.quaternion), quat, 0.2f);		//��O�����͗v�̓p�[�Z���e�[�W�B���̏ꍇ1f��5%���X�������Ă�

	// ����̃N�H�[�^�j�I���̌��ʂ�ۑ�����
	XMStoreFloat4(&g_Player.quaternion, quat);

}

void IncibleEffect(void)
{
	if (g_Player.blinking < 0)
	{
		g_Player.blinkingCount *= -1;
		g_Player.blink -= 1.0f;
	}

	if (g_Player.blinking > g_Player.blinkingMax)
	{
		g_Player.blinkingCount *= -1;
		g_Player.blink += 1.0f;
	}
	g_Player.blinking += g_Player.blinkingCount;
}
