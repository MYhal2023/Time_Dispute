//=============================================================================
//
// �Q�[����ʏ��� [game.h]
// Author : 
//
//=============================================================================
#pragma once
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	FIELD_X				(120)	// X�������̃u���b�N��
#define	FIELD_Z				(120)	// Z�������̃u���b�N��
#define	BLOCK_SIZE			(10.0f)	// �u���b�N�T�C�Y
#define	FIELD_X_LIMIT		(FIELD_X * BLOCK_SIZE * 0.5f)	// X�������̌��E���W
#define	FIELD_Z_LIMIT		(FIELD_Z * BLOCK_SIZE * 0.5f)	// Z�������̌��E���W
#define	WAVE_POS_Y			(-20.0f)	// �g���������Ă���ׁA�ǂ�����������

#define	WALL_Y				(1)	// ���b�V���ǂ̏c����
#define	WALL_XZ				(1)	// ���b�V���ǂ̉�����
#define	WALL_BLOCK_SIZE_Y	(360)	// �u���b�N�T�C�Y�c
#define	WALL_BLOCK_SIZE_XZ	(FIELD_X_LIMIT * 2.0f)	// �u���b�N�T�C�Y��
#define	SCORE_COIN				(100)	// X�������̃u���b�N��
#define	SCORE_TIMECENT			(10)	// X�������̃u���b�N��
#define	SCORE_BEAT				(50)	// X�������̃u���b�N��
#define	ADD_TIME				(5)	// X�������̃u���b�N��

enum
{
	TITLE_NON,
	TUTORIAL,
	MAIN_GAME,
	TUTORIAL_GAME,
	RESULT
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);
void DrawGame0(void);
void DrawGame1(void);
void CheckHit(void);
void CheckModeChange(void);
void SetPlayMode(int playMode);
int GetPlayMode(void);
void SetViewPortType(int viewport);
int GetViewPortTypeGame(void);