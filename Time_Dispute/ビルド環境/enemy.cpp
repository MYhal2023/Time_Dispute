//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "math.h"
#include "renderer.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "collision.h"
#include "player.h"
#include "coin.h"
#include "extension.h"
#include "amadeus.h"
#include "debugproc.h"
#include "rock.h"
#include "time.h"
#include "clone.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	VALUE_MOVE			(1.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define ENEMY_OFFSET_Y		(8.0f)							// �G�l�~�[�̑��������킹��
#define ENEMY_ATK_INTERVAL	(1.0f)							// �G�l�~�[�̍U���Ԋu
#define ENEMY_INTERVAL		(400.0f)						// �G�l�~�[�̐����Ԋu
#define POP_ENEMY_INTERVAL	(40.0f)							// �G�l�~�[�̐����Ԋu������������
#define POP_TIME_ENEMY_INTERVAL	(15.0f)						// �G�l�~�[�̐����Ԋu������������
#define MOTION_INTERVAL		(240.0f)						// �G�l�~�[�̃X�e�[�g�J�ڊԊu
#define ENEMY_EYESIGHT		(200.0f)						// �G�l�~�[�̎���
#define ENEMY_MEMORY		(180.0f)						// �G�l�~�[�̋L������
#define MAX_MEMORY			(10)							// �G�l�~�[�̋L����
#define ENEMY_OFFSET		(40)							// �G�l�~�[�̏o�����W����

#define TPS_INTERVAL		(POINT_INTERVAL)				//��̓C���^�[�o��
#define AT_INTERVAL			(240.0f)
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[
static ENEMY			g_Parts[ENEMY_PARTS_MAX];		// �G�l�~�[�̃p�[�c
static BOOL				g_Load = FALSE;
static float			Pop;							//�G�l�~�[���o���Ƃ��̃C���^�[�o���J�E���g�ϐ�
static float			popInterval;
static float			interval[MAX_ENEMY];			//TPS���Q�Ƃ���Ԋu
static float			frameCount[MAX_ENEMY];
static INTERPOLATION_DATA move_tbl_parts[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 2, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].moveVec = { 0.0f, 0.0f, 0.0f };
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��


		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);

		g_Enemy[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Enemy[i].life = 1;
		g_Enemy[i].lifeMax = g_Enemy[i].life;
		g_Enemy[i].atInterval = 0.0f;
		g_Enemy[i].character = PLANE;
		g_Enemy[i].motion = 0.0f;
		g_Enemy[i].use = FALSE;			// TRUE:�����Ă� �������i�K�ł͖��g�p��

		// �p�[�c�̏�����
		for (int k = 0; k < ENEMY_PARTS_MAX; k++)
		{
			if (g_Parts[k].use == FALSE)
			{
				// �ʒu�E��]�E�X�P�[���̏����ݒ�
				g_Parts[k].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_Parts[k].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_Parts[k].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

				// �e�q�֌W
				g_Parts[k].parent = &g_Enemy[i];		// �����ɐe�̃A�h���X������

				// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
				g_Parts[k].tbl_adr = NULL;				// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
				g_Parts[k].move_time = 0.0f;			// ���s���Ԃ��N���A
				g_Parts[k].tbl_size = 0;				// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
			}
		}

		for (int k = 0; k < ENEMY_PARTS_NUM; k++)		//�p�[�c�������鎖��\�z���āA����ɑΉ������Ă���
		{
			int partsNum = i * ENEMY_PARTS_NUM + k;		//�p�[�c�̓Y���������߂�B���݂̓G�l�~�[�̓Y���������̂܂ܗ���悤�ɂȂ��Ă���
			g_Parts[partsNum].use = TRUE;
			g_Parts[partsNum].parent = &g_Enemy[i];		// �e���Z�b�g

			if (k == 0)
			{
				g_Parts[partsNum].tbl_adr = move_tbl_parts;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
				g_Parts[partsNum].tbl_size = sizeof(move_tbl_parts) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
				g_Parts[partsNum].load = TRUE;
			}
		}
	}

	Pop = 0.0f;				//�G�l�~�[���o���Ƃ��̃C���^�[�o���J�E���g�ϐ�
	popInterval = 0.0f;		//�G�l�~�[�����Ԋu
	g_Load = TRUE;			//�ǂݍ��݊���
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	// �p�[�c�̏�����
	for (int i = 0; i < ENEMY_PARTS_MAX; i++)
	{
		if (g_Parts[i].load)
		{
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}
	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use != TRUE)		// ���̃G�l�~�[���g���Ă��Ȃ��Ȃ珈�����X�L�b�v
			continue;
		//�C���^�[�o���J�E���g�����ȏ゠��΁ATPS�����ɒP�ʃx�N�g�����X�V
		interval[i] += 1.0f;
		if (interval[i] >= TPS_INTERVAL)
		{
			g_Enemy[i].moveVec = TacticalPointSystem(i);
			interval[i] = 0.0f;
			frameCount[i] = 0.0f;
		}
		XMVECTOR now = XMLoadFloat3(&g_Enemy[i].pos);								// ���݂̏ꏊ
		XMStoreFloat3(&g_Enemy[i].pos, now + g_Enemy[i].moveVec * TPS_ENEMY_SPEED);	//�P�ʃx�N�g�������Ɉړ�

		if (frameCount[i] < TPS_ROT_ENEMY_SPEED)
		{
			g_Enemy[i].rot.y += g_Enemy[i].frameRot;
			frameCount[i] += 1.0f;
		}
		else if (frameCount[i] >= TPS_ROT_ENEMY_SPEED)
		{
			g_Enemy[i].rot.y = g_Enemy[i].nextrot.y;
		}
		AttackBullet(i);

		// ���C�L���X�g���đ����̍��������߂�
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
		XMFLOAT3 hitPosition;								// ��_
		hitPosition.y = g_Enemy[i].pos.y - ENEMY_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
		bool ans = RayHitField(g_Enemy[i].pos, &hitPosition, &normal);
		g_Enemy[i].pos.y = hitPosition.y + ENEMY_OFFSET_Y;


		// �p�[�c�̊K�w�A�j���[�V����
		for (int k = 0; k < ENEMY_PARTS_NUM; k++)
		{
			int partsNum = i * ENEMY_PARTS_NUM + k;		//�p�[�c�̓Y���������߂�B���݂̓G�l�~�[�̓Y���������̂܂ܗ���悤�ɂȂ��Ă���
			// �g���Ă���Ȃ珈������
			if ((g_Parts[partsNum].use == TRUE) && (g_Parts[partsNum].tbl_adr != NULL))
			{
				// �ړ�����
				int		index = (int)g_Parts[partsNum].move_time;
				float	time = g_Parts[partsNum].move_time - index;
				int		size = g_Parts[partsNum].tbl_size;

				float dt = 1.0f / g_Parts[partsNum].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_Parts[partsNum].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Parts[partsNum].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Parts[partsNum].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Parts[partsNum].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Parts[partsNum].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Parts[partsNum].scl, s0 + scl * time);

			}
		}
	}
#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Enemy:X:%d C:%d\n", g_Enemy[0].status, g_Enemy[1].status);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Enemy[i].model);

		// �p�[�c�̔��f
		for (int k = 0; k < ENEMY_PARTS_NUM; k++)	//�G�l�~�[�̃p�[�c���������}�g���N�X�𔽉f
		{
			int partsNum = i * ENEMY_PARTS_NUM + k;		//�p�[�c�̓Y���������߂�B���݂̓G�l�~�[�̓Y���������̂܂ܗ���悤�ɂȂ��Ă���
			
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[partsNum].scl.x, g_Parts[partsNum].scl.y, g_Parts[partsNum].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[partsNum].rot.x, g_Parts[partsNum].rot.y, g_Parts[partsNum].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[partsNum].pos.x, g_Parts[partsNum].pos.y, g_Parts[partsNum].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[partsNum].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[partsNum].parent->mtxWorld));
				// ��
				// g_Player.mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Parts[partsNum].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_Parts[partsNum].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// ���f���`��
			DrawModel(&g_Parts[partsNum].model);

		}

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy(void)
{
	return &g_Enemy[0];
}

ENEMY *GetParts(void)
{
	return &g_Parts[0];
}


void SetEnemy(void)
{
	// �G�l�~�[�̐�������
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE)
			continue;

		enemy_count++;
	}
	if (enemy_count >= MAX_ENEMY)//�G�l�~�[���ő吔�Ŋ��ɂ���Ȃ牽�����Ȃ�
		return;
#ifdef DEBUG

	if (GetKeyboardPress(DIK_P))
		return;
#endif

	//�G�l�~�[�X�|�[������
	Pop += 1.0f;
	popInterval = ENEMY_INTERVAL;
	int time = GetTime();
	//���Ԃ��o�قǃG�l�~�[�̐����Ԋu��Z������
	while (time > 0)
	{
		time -= (int)(POP_TIME_ENEMY_INTERVAL);
		popInterval -= POP_ENEMY_INTERVAL;
	}

	if (Pop < popInterval)
		return;
	//�G�l�~�[�����ɔ����ď��������s��
	Pop = 0.0f;
	for (int i = 0; i < MAX_ENEMY; i++)	//���g�p�̔z��ԍ��ɃA�N�Z�X
	{
		if (g_Enemy[i].use != FALSE)	//�g�p�ς݂Ȃ�X�L�b�v
			continue;

		//�o���ꏊ�̐ݒ�
		XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
		//�}�b�v�̒[�ɗ��Ȃ�����Q�����łȂ����W�ԂŃ����_���ɍ��W������
		ROCK *rock = GetRock();
		BOOL ans = TRUE;
		float posX;
		float posZ;
		while (ans == FALSE)
		{
			posX = (float)(ENEMY_OFFSET + rand() % (FIELD_X - (ENEMY_OFFSET * 2 - 1)));
			posZ = (float)(ENEMY_OFFSET + rand() % (FIELD_Z - (ENEMY_OFFSET * 2 - 1)));
			pos.x = -(FIELD_X * 0.5f) * BLOCK_SIZE + (posX * BLOCK_SIZE);
			pos.z = (FIELD_Z * 0.5f) * BLOCK_SIZE - (posZ * BLOCK_SIZE);
			ans = TRUE;
			for (int k = 0; k < MAX_ROCK; k++)
			{
				if ((CollisionBC(pos, rock[k].pos, g_Enemy[i].size, rock[k].size) == TRUE))
					ans = FALSE;
			}
		}
		g_Enemy[i].pos = { pos.x, pos.y, pos.z };
		g_Enemy[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Enemy[i].nextrot = g_Enemy[i].rot;
		g_Enemy[i].scl = { 1.0f, 1.0f, 1.0f };
		g_Enemy[i].use = TRUE;
		g_Enemy[i].moveVec = { 0.0f, 0.0f, 0.0f };
		g_Enemy[i].frameRot = 0.0f;
		g_Enemy[i].life = 1;
		g_Enemy[i].lifeMax = g_Enemy[i].life;
		g_Enemy[i].atInterval = 0.0f;
		g_Enemy[i].atBullet = FALSE;
		g_Enemy[i].character = rand() % MAX;
		ChangeDiffuse(i);

		g_Enemy[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		//�p�[�c�̐ݒ�
		for (int k = 0; k < ENEMY_PARTS_NUM; k++)		//
		{
			int partsNum = i * ENEMY_PARTS_NUM + k;		//�p�[�c�̓Y���������߂�B���݂̓G�l�~�[�̓Y���������̂܂ܗ���悤�ɂȂ��Ă���
			g_Parts[partsNum].use = TRUE;
			g_Parts[partsNum].parent = &g_Enemy[i];		// �e���Z�b�g

			if (k == 0)
			{
				g_Parts[partsNum].tbl_adr = move_tbl_parts;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
				g_Parts[partsNum].tbl_size = sizeof(move_tbl_parts) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
				g_Parts[partsNum].load = TRUE;
			}
		}
		break;
	}
}


void ChangeDiffuse(int i)
{
	//���i���ƂɐF���㏑��
	if (g_Enemy[i].character == PLANE)
	{
		SetModelDiffuse(&g_Enemy[i].model, 0, { 1.0f, 1.0f, 1.0f ,1.0f });
	}
	else if (g_Enemy[i].character == OFFENSIVE)
	{
		SetModelDiffuse(&g_Enemy[i].model, 0, { 2.0f, 0.0f, 0.0f ,1.0f });
	}
	else if (g_Enemy[i].character == TAKE_TIME)
	{
		SetModelDiffuse(&g_Enemy[i].model, 0, { 0.0f, 0.0f, 0.0f ,1.0f });
	}
	else if (g_Enemy[i].character == CROW)
	{
		SetModelDiffuse(&g_Enemy[i].model, 0, { 2.0f, 2.0f, 0.0f ,1.0f });
	}

}

//�e�ۍU���Ɋւ���֐�
void AttackBullet(int i)
{
	PLAYER *player = GetPlayer();
	ROCK *rock = GetRock();
	CLONE *clone = GetClone();
	if (g_Enemy[i].atBullet == FALSE)
		g_Enemy[i].atInterval += 1.0f;
	//��莞�ԂŒe���ăZ�b�g
	if (g_Enemy[i].atInterval > AT_INTERVAL)
	{
		g_Enemy[i].atBullet = TRUE;
	}
	if (g_Enemy[i].atBullet != TRUE)return;

	float point = 10000.0f;					//��r�p�ɕۑ����邽�߂̕ϐ�
	float lenSq = 0.0f;						//�����̑召��}��ׂɕۑ�����ϐ�
	XMFLOAT3 setPos = {0.0f, 0.0f, 0.0f};	//�������W��ۑ����邽�߂̕ϐ�
	//�e���Z�b�g����Ă���Ȃ甭�ˑΏۂ�I��(�T�������̓v���C���[���N���[��)
	//�ː����ʂ��Ă���Ȃ���W�Ƌ�����ۑ�
	if (g_Enemy[i].atBullet == TRUE && GetCrossPoints(g_Enemy[i].pos, player->pos, rock[i].pos, ROCK_SIZE) == FALSE)
	{
		XMVECTOR pos = XMLoadFloat3(&player->pos) - XMLoadFloat3(&g_Enemy[i].pos);
		XMStoreFloat(&lenSq, XMVector3LengthSq(pos));
		XMStoreFloat3(&setPos, pos);
	}

	for (int k = 0; k < MAX_CLONE; k++)
	{
		if (GetCrossPoints(g_Enemy[i].pos, clone[k].pos, rock[i].pos, ROCK_SIZE) == FALSE)
		{
			XMVECTOR pos = XMLoadFloat3(&clone[k].pos) - XMLoadFloat3(&g_Enemy[i].pos);
			XMStoreFloat(&lenSq, XMVector3LengthSq(pos));

			if (point > lenSq)//�����̑召���v��B�������������ق��ɑ_�����߂�
			{
				point = lenSq;
				XMStoreFloat3(&setPos, pos);
			}
		}
	}
	//�Ώۂ����߂��炻���ɒe�𔭎�
	float angle = atan2f(setPos.x, setPos.z);
	SetBullet(g_Enemy[i].pos, XMFLOAT3(0.0f, angle, 0.0f));
	g_Enemy[i].atBullet = FALSE;
	g_Enemy[i].atInterval = 0.0f;

}