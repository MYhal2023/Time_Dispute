//=============================================================================
//
// ����AI���� [salieri.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	CLONE_MIN_VALUE		(0.0f)					// �Œ�]���l
#define	CLONE_MAX_VALUE		(100.0f)				// �ō��]���l
#define TPS_CLONE_SPEED		(1.0f)					//�G�l�~�[�̈ړ����x(��p�ʒu���AI�Ŏg�p)
#define TPS_ROT_CLONE_SPEED (10.0f)					//���t���[���ŃG�l�~�[�̉�]���I��点�邩
#define CLONE_POINT_INTERVAL (20.0f)			//�E�F�C�|�C���g�����Ԋu

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct CLONE_WAYPOINT
{
	XMFLOAT4X4			mtxWorld;		// ���[���h�}�g���b�N�X
	MATERIAL			material;		// �}�e���A��
	XMFLOAT3			pos;			//���W
	XMFLOAT3			rot;			// �p�x
	XMFLOAT3			scl;			// �X�P�[��
	float				value;			//�]���l
	BOOL				use;			//�g�p�̉�(true�Ŏg�p��)
};
//�F��������I�u�W�F�N�g���
enum
{
	CLONE_ITEM_COIN,
	CLONE_ITEM_EXTEN,
	CLONE_ENEMY,
	CLONE_NON
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSalieri(void);
void UninitSalieri(void);
BOOL CloneVisibility(XMFLOAT3 pos1, XMFLOAT3 pos2, float rot, float dist);
XMVECTOR CloneTacticalPointSystem(int i);
BOOL FilterCloneWaypoint(XMFLOAT3 pos, XMFLOAT3 pos2, int k);
float ValueCloneWaypoint(XMFLOAT3 pos, float value, int k);
void ValueCloneCoefficient(int k);
float CloneTacticalSuperiority(XMFLOAT3 pos1, XMFLOAT3 pos2, int k);
void CloneAttack(XMFLOAT3 pos, int k);