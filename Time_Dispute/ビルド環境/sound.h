//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_title,		// BGM0
	SOUND_LABEL_BGM_game,		// 
	SOUND_LABEL_BGM_result,		// 
	SOUND_LABEL_SE_button,		// SE
	SOUND_LABEL_SE_clock,		// 
	SOUND_LABEL_SE_clock_enemy,	// 
	SOUND_LABEL_SE_coin,		// 
	SOUND_LABEL_SE_coin_enemy,	// 
	SOUND_LABEL_SE_enemydown,	// 
	SOUND_LABEL_SE_hit000,		// 
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

