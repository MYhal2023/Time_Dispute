//=============================================================================
//
// ���f������ [player.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/slime.obj"			// �ǂݍ��ރ��f����
#define MAX_PLAYER		(1)					// �v���C���[�̐�

#define	PLAYER_SIZE		(8.0f)				// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3			pos;		// �|���S���̈ʒu
	XMFLOAT3			rot;		// �|���S���̌���(��])
	XMFLOAT3			scl;		// �|���S���̑傫��(�X�P�[��)
	XMFLOAT3			moveVec;	//�ړ��x�N�g���B���K�����Ďg�p
	XMFLOAT4X4			mtxWorld;	// ���[���h�}�g���b�N�X

	BOOL				load;
	DX11_MODEL			model;		// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float				spd;		// �ړ��X�s�[�h
	float				dir;		// ����
	float				size;		// �����蔻��̑傫��
	int					shadowIdx;	// �e��Index
	BOOL				use;

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	INTERPOLATION_DATA	*tbl_adr;	// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;	// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;	// ���s����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	PLAYER				*parent;	// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			quaternion;	// �N�H�[�^�j�I��
	XMFLOAT3			upVector;	// �����������Ă��鏊

	//�U��
	BOOL			attack;			//�U�������ۂ�
	BOOL			attackUse;		//���̍U�����肪�g�p���ꂽ��

	int				life;			//�v���C���[HP
	int				lifeMax;		//�v���C���[�ő�HP

	BOOL			atInvinc;				//���G���Ԓ����ۂ�
	float			atInvincTime;			//���Ԍv���p
	float			blink;
	float			blinking;
	float			blinkingMax;
	float			blinkingCount;

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);
void SettingPlayer(void);
void IncibleEffect(void);