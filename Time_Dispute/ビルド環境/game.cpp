//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "light.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "title.h"
#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "rock.h"
#include "bullet.h"
#include "score.h"
#include "interface.h"
#include "life.h"
#include "time.h"
#include "coin.h"
#include "extension.h"
#include "amadeus.h"
#include "collision.h"
#include "debugproc.h"
#include "salieri.h"
#include "clone.h"
#include "money.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// �|�[�YON/OFF
static int	g_PlayMode = TITLE_NON;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	switch (GetMode())
	{
	case MODE_TITLE:
	case MODE_GAME:
		g_ViewPortType_Game = TYPE_FULL_SCREEN;

		InitTitle();

		// �t�B�[���h�̏�����
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), FIELD_X, FIELD_Z, BLOCK_SIZE, BLOCK_SIZE, WATER);

		// ���C�g��L����	// �e�̏���������
		InitShadow();

		// �v���C���[�̏�����
		InitPlayer();

		// �G�l�~�[�̏�����
		InitEnemy();

		// �ǂ̏�����
		InitMeshWall(XMFLOAT3(0.0f, WAVE_POS_Y, FIELD_Z_LIMIT), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(-FIELD_X_LIMIT, WAVE_POS_Y, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(FIELD_X_LIMIT, WAVE_POS_Y, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(0.0f, WAVE_POS_Y, -FIELD_Z_LIMIT), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);

		// ��(�����p�̔�����)
		InitMeshWall(XMFLOAT3(0.0f, WAVE_POS_Y, FIELD_Z_LIMIT), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(-FIELD_X_LIMIT, WAVE_POS_Y, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(FIELD_X_LIMIT, WAVE_POS_Y, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);
		InitMeshWall(XMFLOAT3(0.0f, WAVE_POS_Y, -FIELD_Z_LIMIT), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f), WALL_XZ, WALL_Y, WALL_BLOCK_SIZE_XZ, WALL_BLOCK_SIZE_Y);

		//����o��
		InitRock();

		// �e�̏�����
		InitBullet();

		// �X�R�A�̏�����
		InitScore();

		// �������̏�����
		InitMoney();

		// ���C�t�̏�����
		InitLife();

		// �������Ԃ̏�����
		InitTime();

		//UI�\��������
		InitInterface();

		//�R�C���̏�����
		InitCoin();

		//�����A�C�e���̏�����
		InitExten();

		//�GAI������
		InitAmadeus();

		//�����N���[��������
		InitClone();

		//����AI������
		InitSalieri();


		//BGM�Đ�
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{

	//UI�\���I������
	UninitInterface();

	// �X�R�A�̏I������
	UninitScore();

	//�������̏I������
	UninitMoney();

	// ���C�t�̏I������
	UninitLife();

	// �������Ԃ̏I������
	UninitTime();

	//�R�C���̏I������
	UninitCoin();

	//�����A�C�e���̏I������
	UninitExten();

	// �e�̏I������
	UninitBullet();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	//�^�C�g����ʂ̏I������
	UninitTitle();

	// �e�̏I������
	UninitShadow();

	UninitSalieri();
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;
	PLAYER *player = GetPlayer();
	UpdateTitle();

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �Ǐ����̍X�V
	UpdateMeshWall();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	UpdateLight();

	// �e�̍X�V����
	UpdateShadow();

	// �����蔻�菈��
	CheckHit();

	if (g_PlayMode <= TUTORIAL)	//�^�C�g���Ȃ炱���܂ōX�V
		return;

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	UpdateClone();

	//��̍X�V����
	UpdateRock();

	// �e�̍X�V����
	UpdateBullet();

	//�R�C���̍X�V����
	UpdateCoin();

	//�����A�C�e���̍X�V����
	UpdateExten();

	// �X�R�A�̍X�V����
	UpdateScore();

	//�������̍X�V����
	UpdateMoney();

	//UI�\���X�V����
	UpdateInterface();

	// ���C�t�̍X�V����
	UpdateLife();

	// �������Ԃ̍X�V����
	UpdateTime();
	
	CheckModeChange();

	//�e�I�u�W�F�N�g��ݒu
	SetCoin();
	SetExten();
	SetEnemy();
}

//=============================================================================
// �`�揈��(�J�����ڐ�)
//=============================================================================
void DrawGame0(void)
{
	PLAYER *player = GetPlayer();
	//SetCamera();
	//�V�F�[�_�[�Ǘ�
	//�|�X�g�G�t�F�N�g��������ꍇ�͂�������
	int ans = MODE_PLANE;

	if (GetTime() <= 0 || player->life <= 0)
		ans = MODE_MONO;

	SwapShader(ans);

	// 3D�̕���`�悷�鏈��
	//�V�F�[�_�[�؂�ւ��B���ʂ̕`�揈��
	ans = MODE_SPECULAR;
	if (GetTime() <= 0 || player->life <= 0)
		ans = MODE_MONO;
	SwapShader(ans);

	// �n�ʂ̕`�揈��
	DrawMeshField();
	//�V�F�[�_�[�؂�ւ��B�m�[�}���̕��ɖ߂�
	ans = MODE_PLANE;
	if (GetTime() <= 0 || player->life <= 0)
		ans = MODE_MONO;

	SwapShader(ans);

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	DrawClone();

	// �e�̕`�揈��
	DrawBullet();

	//��̕`�揈��
	DrawRock();

	//�R�C���̕`�揈��
	DrawCoin();

	//�����A�C�e���̕`�揈��
	DrawExten();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	DrawTitle();

	if (g_PlayMode > TUTORIAL)
	{
		// �X�R�A�̕`�揈��
		DrawScore();

		//�������̕`�揈��
		DrawMoney();

		// ���C�t�̕`�揈��
		DrawLife();

		// �������Ԃ̕`�揈��
		DrawTime();

		//UI�\���`�揈��
		DrawInterface();
	}

	//�V�F�[�_�[�Ǘ�
	//�V�F�[�_�[�����ɖ߂��B�|�X�g�G�t�F�N�g�͂����܂�
	ans = MODE_PLANE;
	SwapShader(ans);
	if (GetTime() <= 0 || player->life <= 0)
		DrawGameOver();

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);
}

//=============================================================================
// �`�揈��(���C�g�ڐ��B�����������e�Ɋւ���I�u�W�F�N�g�������w��)
//=============================================================================
void DrawGame1(void)
{
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	//�����̕`�揈��
	DrawClone();

	//��̕`�揈��
	DrawRock();
}


void DrawGame(void)
{


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif
	XMFLOAT3 pos;

	// �v���C���[���_
	pos = GetPlayer()->pos;
	//pos = GetEnemy()->pos;	//�f�o�b�O�p
	pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		DrawGame0();
		break;

	case TYPE_LIGHT_SCREEN:	//���C�g�ڐ��̕`������邽�߂̃r���[�|�[�g�^�C�v
		DrawGame1();
		break;

	}

}


//=============================================================================
// �I�u�W�F�N�g���m�̓����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	BULLET *bullet = GetBullet();	// �e�̃|�C���^�[��������
	COIN *coin = GetCoin();
	EXTEN *exten = GetExten();
	CLONE *clone = GetClone();
	// �G�l�~�[�ƃL�����N�^�[
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�l�~�[�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE)
			continue;
		//BC�̓����蔻��
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size) == TRUE)
		{
			if (player->attackUse == TRUE)//�v���C���[���U��
			{
				// �G�l�~�[������
				enemy[i].use = FALSE;
				ReleaseShadow(enemy[i].shadowIdx);
				player->attackUse = FALSE;	//�v���C���[�̍U�����g�p�ς݂ɂ���
				AddScore(SCORE_BEAT);
				PlaySound(SOUND_LABEL_SE_enemydown);
				break;
			}
			else if (player->attackUse == FALSE && player->attack == FALSE && player->atInvinc == FALSE)	//�U�����g�p�ς݂��U�����[�V�������łȂ��A���G���Ԓ��ł��Ȃ�
			{
				AddLife(-1, PLAYER_LIFE, 0);	// �v���C���[�̃��C�t�����炷
				player->atInvinc = TRUE;		//���G���Ԃɓ���
				PlaySound(SOUND_LABEL_SE_hit000);
				break;
			}
		}
	}

	// �G�l�~�[�ƃN���[��
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�l�~�[�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE)
			continue;
		for (int k = 0; k < MAX_CLONE; k++)
		{
			//BC�̓����蔻��
			if (CollisionBC(clone[k].pos, enemy[i].pos, clone[k].size, enemy[i].size) != TRUE)
				continue;

			if (clone[k].attackUse == TRUE)			//�N���[�����U����
			{
				// �G�l�~�[������
				enemy[i].use = FALSE;
				clone[k].attackUse = FALSE;			//�U�����g�p�ς݂ɂ���
				AddScore((int)(SCORE_BEAT * 0.5));
				//PlaySound(SOUND_LABEL_SE_enemydown);
				break;
			}
			else if (clone[k].attackUse == FALSE && clone[k].attack == FALSE && clone[k].atInvinc == FALSE)	//�U�����g�p�ς݂��U�����[�V�������łȂ��A���G���Ԓ��ł��Ȃ�
			{
				AddLife(-1, CLONE_LIFE, k);			//���C�t�����炷
				clone[k].atInvinc = TRUE;			//���G���Ԃɓ���
				//PlaySound(SOUND_LABEL_SE_hit000);
				break;
			}

		}
	}
	// �G�l�~�[�̒e�ƃL�����N�^�[
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (bullet[i].use == FALSE)
			continue;
		//BC�̓����蔻��A���G���Ȃ珈�����X�L�b�v
		if (CollisionBC(bullet[i].pos, player->pos, bullet[i].fWidth, player->size) == TRUE && player->atInvinc == FALSE)
		{
			// �����������疢�g�p�ɖ߂�
			bullet[i].use = FALSE;
			ReleaseShadow(bullet[i].shadowIdx);
			AddLife(-1, PLAYER_LIFE, 0);	//�v���C���[�̃��C�t�����炷
			player->atInvinc = TRUE;		//���G���Ԃɓ���
			PlaySound(SOUND_LABEL_SE_hit000);
			break;
		}
		//�N���[�����`�F�b�N
		for (int k = 0; k < MAX_CLONE; k++)
		{
			if (CollisionBC(bullet[i].pos, clone[k].pos, bullet[i].fWidth, clone[k].size) != TRUE || clone[k].atInvinc != FALSE)
				continue;

			// �����������疢�g�p�ɖ߂�
			bullet[i].use = FALSE;
			ReleaseShadow(bullet[i].shadowIdx);
			AddLife(-1, CLONE_LIFE, k);	//�v���C���[�̃��C�t�����炷
			clone[k].atInvinc = TRUE;		//���G���Ԃɓ���
			//PlaySound(SOUND_LABEL_SE_hit000);
			break;
		}
	}

	// �R�C���ƃL�����N�^�[
	for (int i = 0; i < MAX_COIN; i++)
	{
		//�R�C���̗L���t���O���`�F�b�N����
		if (coin[i].use == FALSE)
			continue;
		//�R�C���ƃv���C���[
		if (CollisionBC(coin[i].pos, player->pos, coin[i].fWidth, player->size) == TRUE)
		{
			// �����������疢�g�p�ɖ߂�
			coin[i].use = FALSE;
			ReleaseShadow(coin[i].shadowIdx);
			// �v���C���[�̃X�R�A�Ə������𑝂₷
			AddScore(SCORE_COIN);
			AddMoney(10);
			PlaySound(SOUND_LABEL_SE_coin);
			break;
		}

		//�����ăN���[�����`�F�b�N
		for (int k = 0; k < MAX_CLONE; k++)
		{
			if (CollisionBC(coin[i].pos, clone[k].pos, coin[i].fWidth, clone[k].size) != TRUE)
				continue;

			// �����������疢�g�p�ɖ߂�
			coin[i].use = FALSE;
			ReleaseShadow(coin[i].shadowIdx);
			// �v���C���[�̃X�R�A�Ə������𑝂₷
			AddScore((int)(SCORE_COIN * 0.5));
			AddMoney(10);
			//PlaySound(SOUND_LABEL_SE_coin);
			break;
		}
		//�Ō�ɃG�l�~�[���`�F�b�N
		for (int n = 0; n < MAX_ENEMY; n++)
		{
			//�G�l�~�[�̗L���t���O���`�F�b�N����
			if (enemy[n].use == FALSE)
				continue;

			//BC�̓����蔻��
			if (CollisionBC(coin[i].pos, enemy[n].pos, coin[i].fWidth, enemy[n].size) != TRUE)
				continue;

			// �����������疢�g�p�ɖ߂�
			coin[i].use = FALSE;
			ReleaseShadow(coin[i].shadowIdx);	//�e�̉������
			// �v���C���[�̃X�R�A�����炷
			AddScore(-(int)(SCORE_COIN * 0.5));
			AddMoney(-5);
			PlaySound(SOUND_LABEL_SE_coin_enemy);
			break;
		}

	}

	// ���ԉ����A�C�e���ƃL�����N�^�[
	for (int i = 0; i < MAX_EXTEN; i++)
	{
		//���ԉ����A�C�e���̗L���t���O���`�F�b�N����
		if (exten[i].use == FALSE)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(exten[i].pos, player->pos, exten[i].fWidth, player->size) == TRUE)
		{
			// �����������疢�g�p�ɖ߂�
			exten[i].use = FALSE;
			ReleaseShadow(exten[i].shadowIdx);
			// �������Ԃ̉���
			AddTime((int)(ADD_TIME * 2));
			PlaySound(SOUND_LABEL_SE_clock);
			break;
		}
		//�����ăN���[�����`�F�b�N
		for (int k = 0; k < MAX_CLONE; k++)
		{
			if (CollisionBC(exten[i].pos, clone[k].pos, exten[i].fWidth, clone[k].size) != TRUE)
				continue;

			// �����������疢�g�p�ɖ߂�
			exten[i].use = FALSE;
			ReleaseShadow(exten[i].shadowIdx);
			// �������Ԃ̉���
			AddTime(ADD_TIME);
			//PlaySound(SOUND_LABEL_SE_clock);
			break;
		}
		//�Ō�ɃG�l�~�[���`�F�b�N
		for (int n = 0; n < MAX_ENEMY; n++)
		{
			//�G�l�~�[�̗L���t���O���`�F�b�N����
			if (enemy[n].use == FALSE)
				continue;

			//BC�̓����蔻��
			if (CollisionBC(exten[i].pos, enemy[n].pos, exten[i].fWidth, enemy[n].size) != TRUE)
				continue;

			// �����������疢�g�p�ɖ߂�
			exten[i].use = FALSE;
			ReleaseShadow(exten[i].shadowIdx);	//�e�̉������
			// �v���C���[�̃X�R�A�����炷
			AddTime(-ADD_TIME);
			PlaySound(SOUND_LABEL_SE_clock_enemy);
			break;
		}
	}
}

void CheckModeChange(void)
{
	PLAYER *player = GetPlayer();
	if (GetPlayMode() == TUTORIAL_GAME)
	{
		if (GetKeyboardTrigger(DIK_X) || IsButtonTriggered(0, BUTTON_B))
		{
			SetFade(FADE_OUT, MODE_RESULT);
		}
	}


	//���Ԑ؂ꂩ���C�t�؂�Ń��U���g��
	if (GetTime() > 0 && player->life > 0)	
		return;

	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}
}

void SetPlayMode(int playMode)
{
	g_PlayMode = playMode;
}

int GetPlayMode(void)
{
	return g_PlayMode;
}

void SetViewPortType(int viewport)
{
	g_ViewPortType_Game = viewport;
}

int GetViewPortTypeGame(void)
{
	return g_ViewPortType_Game;
}