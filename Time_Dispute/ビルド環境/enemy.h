//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/Eden.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_PARTS	"data/MODEL/Eden_origin.obj"	// �ǂݍ��ރ��f����
#define MAX_ENEMY		(12)									// �G�l�~�[�̐�
#define ENEMY_PARTS_NUM		(1)								// 1�G�l�~�[�̃p�[�c�̐�
#define ENEMY_PARTS_MAX		(MAX_ENEMY * ENEMY_PARTS_NUM)	// �G�l�~�[�̕K�v���p�[�c��

#define	ENEMY_SIZE		(10.0f)								// �����蔻��̑傫��
#define	ENEMY_BATLE_SIZE (150.0f)							// �v���C���[�T������̑傫��
#define DEFER				(2.0f)							// �G�l�~�[�ړ������̗]�蕝
#define ENEMY_MIN_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + DEFER * BLOCK_SIZE)				// �G�l�~�[�ړ����
#define ENEMY_MIN_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - (FIELD_Z - DEFER) * BLOCK_SIZE)	// 
#define ENEMY_MAX_X	(-(FIELD_X / 2.0f) * BLOCK_SIZE + (FIELD_X - DEFER) * BLOCK_SIZE )	// 
#define ENEMY_MAX_Z	((FIELD_Z / 2.0f) * BLOCK_SIZE - DEFER * BLOCK_SIZE)				// 

//�G�l�~�[�̐��i
enum
{
	PLANE,		//�v���[��
	OFFENSIVE,	//�U���I
	TAKE_TIME,	//���ԒD��
	CROW,		//�X�R�A�A�C�e���D��
	MAX			//�ő�l(�Q�Ƃ��Ȃ�)
};
//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
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
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�
	
	INTERPOLATION_DATA	*tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;			// ���s����

	//���C�t
	int					life;
	int					lifeMax;

	//AI
	float				atInterval;			//�G�l�~�[�̍U���Ԋu
	BOOL				atBullet;
	int					character;			//���i
	float				playerValue;		//�v���C���[�]���l
	float				extenValue;			//���ԗD��]���l
	float				scoreValue;			//�X�R�A�D��]���l

	int					status;				//���X�e�[�g
	float				motion;				//�X�e�[�g�J��
	// �e�́ANULL�A�q���͐e�̃A�h���X������
	ENEMY				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
ENEMY *GetParts(void);
void SetEnemy(void);
void ChangeDiffuse(int i);
void AttackBullet(int i);