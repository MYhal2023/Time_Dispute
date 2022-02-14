//=============================================================================
//
// ���C�t���� [life.h]
// Author : GP11B132 34 �đq �r�N
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define LIFE_DIGIT			(2)		// �n�[�g��Ƀ��C�t�������K�v�Ȃ̂�

#define	LIFE_DX			(72.0f)		// �\���ʒu
#define	LIFE_DY			(76.0f)		// 
#define	LIFE_DW			(32.0f)		// 
#define	LIFE_DH			(32.0f)		// 

#define	LIFE_SCL		(1.0f)					// LIFE���̔{��
#define	LIFE_BW			(57.0f * LIFE_SCL)		// LIFE���̉���
#define	LIFE_BH			(17.0f * LIFE_SCL)		// �c��
#define	GAMEOVER_DW		(431.0f)				// �e�N�X�`���̉���
#define	GAMEOVER_DH		(60.0f)					//�e�N�X�`���̏c��
//���C�t����
enum
{
	PLAYER_LIFE,
	ENEMY_LIFE,
	CLONE_LIFE
};
//�e�N�X�`���i���o�[
enum
{
	HART,
	HART_LOST,
	HART_BOS,
	GAMEOVER
};
struct LIFE
{
	XMFLOAT3		pos;		// �|���S���̍��W
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�

	int				life;		// ���C�t
	int				life_max;	//���C�t�̍ő�l
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitLife(void);
void UninitLife(void);
void UpdateLife(void);
void DrawLife(void);
void DrawGameOver(void);
int GetLife(void);
void AddLife(int add, int chara, int i);
void SetLife(int life);