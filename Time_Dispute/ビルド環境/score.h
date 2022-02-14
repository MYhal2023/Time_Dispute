//=============================================================================
//
// �X�R�A���� [score.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCORE_MAX			(99999)		// �X�R�A�̍ő�l
#define SCORE_MIN			(0)			// �X�R�A�̍ŏ��l
#define SCORE_DIGIT			(7)			// ����
#define SCORE_SAVE			(10)		// �X�R�A�̕ۑ���




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

void AddScore(int add);
int GetScore(void);
int GetHighScore(void);
int GetSaveScore(int i);
void SetScore(int score);
void SetHighScore(int score);
void SetSaveScore(int score, int i);