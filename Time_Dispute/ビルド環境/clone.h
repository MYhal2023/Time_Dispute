//=============================================================================
//
// �N���[�����f������ [enemy.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_CLONE			"data/MODEL/Slime.obj"			// �ǂݍ��ރ��f����
#define MAX_CLONE		(5)									// �G�l�~�[�̐�
#define CLONE_PARTS_NUM		(1)								// 1�G�l�~�[�̃p�[�c�̐�
#define CLONE_PARTS_MAX		(MAX_CLONE * CLONE_PARTS_NUM)	// �G�l�~�[�̕K�v���p�[�c��

#define	CLONE_SIZE		(10.0f)								// �����蔻��̑傫��
#define	CLONE_BATLE_SIZE (150.0f)							// �v���C���[�T������̑傫��
#define CLONE_DEFER				(2.0f)							// �G�l�~�[�ړ������̗]�蕝
#define CLONE_MIN_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + CLONE_DEFER * BLOCK_SIZE)				// �G�l�~�[�ړ����
#define CLONE_MIN_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - (FIELD_Z - CLONE_DEFER) * BLOCK_SIZE)	// 
#define CLONE_MAX_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + (FIELD_X - CLONE_DEFER) * BLOCK_SIZE )	// 
#define CLONE_MAX_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - CLONE_DEFER * BLOCK_SIZE)				// 
#define CLONE_OFFENSIVE_VALUE (40)
#define CLONE_TAKE_TIME_VALUE (80)
#define CLONE_CROW_VALUE (100)

//�N���[���̐��i
enum
{
	CLONE_OFFENSIVE,	//�U���I
	CLONE_ITEM,			//�A�C�e���̂�
	CLONE_ALL_TAKE,		//�S�Ď擾
	CLONE_MAX			//�ő�l(�Q�Ƃ��Ȃ�)
};
//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct CLONE
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			nextrot;			// ���f���̎��̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)
	XMVECTOR			moveVec;			//���f���̓�������
	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	//AI���痈��ړ����
	float				frameRot;			//1�t���[�����Ƃ̉�]��

	float				size;				// �����蔻��̑傫��
	float				spd;
	//���C�t
	int					life;
	int					lifeMax;

	//AI
	int					character;			//���i
	//�U��
	BOOL				attack;			//�U�������ۂ�
	BOOL				attackUse;		//���̍U�����肪�g�p���ꂽ��

	BOOL				atInvinc;				//���G���Ԓ����ۂ�
	float				atInvincTime;			//���Ԍv���p
	float				blink;
	float				blinking;
	float				blinkingMax;
	float				blinkingCount;

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitClone(void);
void UninitClone(void);
void UpdateClone(void);
void DrawClone(void);

CLONE *GetClone(void);
void SetClone(XMFLOAT3 pos, int character);
void CloneChangeDiffuse(int i);
void CloneIncibleEffect(int i);