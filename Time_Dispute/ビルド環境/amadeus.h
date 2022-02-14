//=============================================================================
//
// �G�l�~�[AI���� [amadeus.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MIN_VALUE			(0.0f)					// �Œ�]���l
#define	MAX_VALUE			(100.0f)				// �ō��]���l
#define TPS_ENEMY_SPEED		(1.0f)					//�G�l�~�[�̈ړ����x(��p�ʒu���AI�Ŏg�p)
#define TPS_ROT_ENEMY_SPEED (10.0f)					//���t���[���ŃG�l�~�[�̉�]���I��点�邩
#define POINT_INTERVAL		(20.0f)					//�E�F�C�|�C���g�����Ԋu

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct WAYPOINT
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
	ITEM_COIN,
	ITEM_EXTEN,
	MAIN_PLAYER,
	NON
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitAmadeus(void);
void UninitAmadeus(void);
BOOL Visibility(XMFLOAT3 pos1, XMFLOAT3 pos2, float rot, float dist);
XMVECTOR TacticalPointSystem(int i);
BOOL FilterWayPoint(XMFLOAT3 pos, XMFLOAT3 pos2, int k);
float ValueWayPoint(XMFLOAT3 pos, float value, int k);
void ValueCoefficient(int k);
float TacticalSuperiority(XMFLOAT3 pos1, XMFLOAT3 pos2, int k);