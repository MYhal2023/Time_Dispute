//=============================================================================
//
// ����AI���� [salieri.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "salieri.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "collision.h"
#include "player.h"
#include "clone.h"
#include "rock.h"
#include "coin.h"
#include "extension.h"
#include "time.h"
#include "score.h"
#include "debugproc.h"
#include "clone.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define EYE_VALUE		(3.0f)				//���_�̏�Z�]���l
#define BACK_VALUE		(0.5f)				//�w�ʂ̏�Z�]���l
//��������AI�Ɏg�p�����`
#define CLONE_EYE_ANGLE		(XM_PI * 0.25f)	//1�����̊p�x
#define CLONE_VISION	(0.25f)				//���E
//TCP�p�̒萔����
#define AI_WAYAREA_SIZE	(5)					//�E�F�C�|�C���g�����p�萔
#define WAYAREA_SIZE	((AI_WAYAREA_SIZE * 2) - 1)	//�E�F�C�|�C���g�����G���A�̈�ӂ̃|�C���g��(�O���b�h��̐����`)
#define NEAR_CLONE		(30.0f)				//�E�F�C�|�C���g�ɋ߂��Ƃ͂ǂꂭ�炢�̋������w���̂���`
#define VALUE_VAR		(3)					//�]���Ώۂ̐�
#define EYE_VALUE_TPS	(1.2f)				//���_�̕]���W��
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CLONE_WAYPOINT	g_Clone_Waypoint[WAYAREA_SIZE][WAYAREA_SIZE];// �E�F�C�|�C���g
static BOOL				g_Load = FALSE;

static float			valueCoe[VALUE_VAR];					//�]���W���̕ۑ���
//=============================================================================
// ����������(�S�Ă̏����������̌�Ɏ��s�����邱��)
//=============================================================================
HRESULT InitSalieri(void)
{
	for (int i = 0; i < WAYAREA_SIZE; i++)
	{
		for (int k = 0; k < WAYAREA_SIZE; k++)
		{
			ZeroMemory(&g_Clone_Waypoint[i][k].material, sizeof(g_Clone_Waypoint[i][k].material));
			g_Clone_Waypoint[i][k].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
			g_Clone_Waypoint[i][k].pos = { 0.0f, 0.0f, 0.0f };			//���W
			g_Clone_Waypoint[i][k].rot = { 0.0f, 0.0f, 0.0f };			// �p�x
			g_Clone_Waypoint[i][k].scl = { 1.0f, 1.0f, 1.0f };			// �X�P�[��
			g_Clone_Waypoint[i][k].value = 1.0f;			//�]���l
			g_Clone_Waypoint[i][k].use = FALSE;			//�g�p�̉�(TRUE�Ŏg�p��)
		}
	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSalieri(void)
{
	if (g_Load == FALSE) return;


	g_Load = FALSE;
}

//========================================================================================================================
// �Ώۂ�ڎ��o���Ă���̂�(����1:�����̈ʒu,����2:�Ώۂ̈ʒu,����3:�����̊p�x,����4:�ǂꂭ�炢�̋����Ȃ�ڎ��ł���̂�)
//========================================================================================================================
BOOL CloneVisibility(XMFLOAT3 pos1, XMFLOAT3 pos2, float rot, float dist)
{
	BOOL ans = FALSE;

	float len = (dist) * (dist);		// ���a��2�悵����
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2�_�Ԃ̋����i2�悵�����j
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);
	// ���a��2�悵������苗��������
	if (len <= lenSq)
	{
		return ans;	// �ڎ��O�ɂ��邽�߂��̎��_�Ōv�Z�I��
	}

	//�ΏۂƂ̊p�x���Z�o
	float angleX = pos2.x - pos1.x;
	float angleZ = pos2.z - pos1.z;
	float angle = atan2f(angleX, angleZ);
	//���g�̊p�x��8�����ŎZ�o
	float stVision = 0.0f;
	float edVision = 0.0f;
	//angle��XM_PI�`-XMPI�Ȃ̂ł���ɍ��킹��
	//���E���l�̐؂�ڂɋ��܂�Ə����ʓ|�ȏ������K�v
	if ((rot > XM_PI * (1.0f - CLONE_VISION)) || (rot < XM_PI * (CLONE_VISION - 1.0f)))
	{
		if (rot > XM_PI * (1.0f - CLONE_VISION))
		{
			stVision = rot - XM_PI * CLONE_VISION;
			edVision = (rot + XM_PI * CLONE_VISION) - (XM_PI * 2.0f);
		}
		else if (rot < XM_PI * (CLONE_VISION - 1.0f))
		{
			stVision = rot - XM_PI * CLONE_VISION + (XM_PI * 2.0f);
			edVision = rot + XM_PI * CLONE_VISION;
		}

		if (angle >= stVision || angle <= edVision)
		{
			ans = TRUE;
		}
	}
	else
	{
		stVision = rot - XM_PI * CLONE_VISION;
		edVision = rot + XM_PI * CLONE_VISION;
		if (angle >= stVision && angle <= edVision)
		{
			ans = TRUE;
		}
	}

	return ans;
}



XMVECTOR CloneTacticalPointSystem(int i)	//��p�ʒu��̓V�X�e���B�z��ԍ��������Ă��邱��
{
	CLONE *clone = GetClone();
	PLAYER *player = GetPlayer();
	ROCK *rock = GetRock();
	int stWayArea = 1 - AI_WAYAREA_SIZE;
	int edWayArea = AI_WAYAREA_SIZE;
	//��������E��ɂ����ăE�F�C�|�C���g������B������
	for (int h = stWayArea; h < edWayArea; h++)
	{
		for (int k = stWayArea; k < edWayArea; k++)
		{
			int a = h + (-stWayArea);
			int b = k + (-stWayArea);
			float x = clone[i].pos.x + CLONE_POINT_INTERVAL * k;
			float y = 0.0f;
			float z = clone[i].pos.z + CLONE_POINT_INTERVAL * h;
			g_Clone_Waypoint[a][b].pos = { x, y, z };
			//���W����Ƌ��Ƀt�B���^�����O�t�F�[�Y��
			g_Clone_Waypoint[a][b].use = FilterCloneWaypoint(g_Clone_Waypoint[a][b].pos, clone[i].pos, i);
			g_Clone_Waypoint[a][b].value = 1.0f;

			//�E�F�C�|�C���g�]���t�F�[�Y
			if (g_Clone_Waypoint[a][b].use == TRUE)
				g_Clone_Waypoint[a][b].value = ValueCloneWaypoint(g_Clone_Waypoint[a][b].pos, 0.0f, i);

		}
	}

	//�c�����E�F�C�|�C���g�̕]���l���r
	float valueMax = 0.0f;		//�ő�]���l��ۑ�
	int value_x = 0;			//�ő�]���l�̔z��ԍ���ۑ�
	int value_z = 0;			//
	for (int a = 0; a < WAYAREA_SIZE; a++)
	{
		for (int b = 0; b < WAYAREA_SIZE; b++)
		{
			if (g_Clone_Waypoint[a][b].use == FALSE)continue;	//FALSE�̏ꍇ�A�폜�����E�F�C�|�C���g�Ȃ̂ŏ������X�L�b�v

			if (valueMax < g_Clone_Waypoint[a][b].value)
			{
				valueMax = g_Clone_Waypoint[a][b].value;
				value_z = a;
				value_x = b;
			}
		}
	}
	//g_Clone_Waypoint[value_z][value_x].material.Diffuse = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMVECTOR vec = { 0.0f, 0.0f, 0.0f };
	if (valueMax == 0.0f) //�ǂ��ɂ��ړ���₪�����ꍇ�͂��̂܂ܕԂ�
		return vec;

	//�ړ���
	float vecX = g_Clone_Waypoint[value_z][value_x].pos.x - clone[i].pos.x;
	float vecZ = g_Clone_Waypoint[value_z][value_x].pos.z - clone[i].pos.z;
	//X����Z�������ł̈ړ��x�N�g�����v�Z
	g_Clone_Waypoint[value_z][value_x].pos.y = 0.0f;
	clone[i].pos.y = 0.0f;
	vec = XMLoadFloat3(&g_Clone_Waypoint[value_z][value_x].pos) - XMLoadFloat3(&clone[i].pos);
	//�P�ʃx�N�g�����B����ɃI�u�W�F�N�g�̃X�s�[�h����Z���Ĉړ�������
	vec = XMVector3Normalize(vec);
	clone[i].moveVec = vec;

	//�����W�ƃE�F�C�|�C���g�̊ԂɓG������Ȃ�U��
	CloneAttack(g_Clone_Waypoint[value_z][value_x].pos, i);

	//�p�x���Z�o���ύX
	float angle = atan2f(vecX, vecZ);
	//��]�ʂ�x���ɕϊ��B�������̕���������₷������
	float degrees = XMConvertToDegrees(angle) - XMConvertToDegrees(clone[i].rot.y);
	if (degrees > 180.0f)
		degrees -= 360.0f;
	else if (degrees < -180.0f)
		degrees += 360.0f;

	clone[i].frameRot = XMConvertToRadians(degrees) / TPS_ROT_CLONE_SPEED;
	clone[i].nextrot.y = angle;
	return vec;
}

//�E�F�C�|�C���g���t�B���^�����O����֐��B�|�C���g�̍��W�Ǝ��g�̔z��ԍ��������Ă���
BOOL FilterCloneWaypoint(XMFLOAT3 pos, XMFLOAT3 pos2, int k)		//�E�F�C�|�C���g�A���g�̌����W�A���g�̔z��ԍ�
{
	BOOL ans = TRUE;							//�܂���TRUE���Z�b�g�B�������牽�ɂ�����������Ȃ����TRUE���Ԃ�A�E�F�C�|�C���g�Ƃ��Ďg�p����
	CLONE *clone = GetClone();
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	ROCK *rock = GetRock();
	//�N���s�̏ꏊ���폜
	//���b�V���t�B�[���h�͈͊O�ɏo�Ȃ��悤�ɂ���
	if (pos.x <= CLONE_MIN_X ||
		pos.x >= CLONE_MAX_X ||
		pos.z <= CLONE_MIN_Z ||
		pos.z >= CLONE_MAX_Z)
	{
		ans = FALSE;
		return ans;
	}

	for (int i = 0; i < MAX_ROCK; i++)
	{
		//���g�̍��W�ƃE�F�C�|�C���g�̐�������ɓ�����ꍇ�A�ʍs�s�̏ꏊ��ʂ�K�v������ׁAFALSE��Ԃ�
		if (GetCrossPoints(pos2, pos, rock[i].pos, ROCK_SIZE + clone[k].size + 1.0f) == TRUE)
		{
			ans = FALSE;
			return ans;
		}
	}

	//�A�C�e���D��̃L�����͓G�̋߂��̃|�C���g���폜
	if (clone[k].character == CLONE_ITEM)
	{
		//���G�l�~�[���߂��ꍇ�͍폜
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			if (CollisionBC(enemy[i].pos, pos, clone[i].size, NEAR_CLONE) == TRUE)
			{
				ans = FALSE;						//���G�l�~�[���߂�����FALSE��Ԃ�
				return ans;
			}
		}
	}

	return ans;
}

//�E�F�C�|�C���g��]������֐��B���g�̔z��ԍ��������Ă���
//����1:�E�F�C�|�C���g���W�A����2:�ŏ��ɂ���]���l�A����3:�z��ԍ�
float ValueCloneWaypoint(XMFLOAT3 pos, float value, int k)
{
	COIN *coin = GetCoin();
	EXTEN *exten = GetExten();
	PLAYER *player = GetPlayer();
	CLONE *clone = GetClone();
	ENEMY *enemy = GetEnemy();
	ValueCloneCoefficient(k);
	for (int i = 0; i < MAX_COIN; i++)
	{
		if (valueCoe[CLONE_ITEM_COIN] == 0.0f || coin[i].use != TRUE)		//�]���W����0�������̓t�B�[���h�ɖ����Ȃ�v�Z�s�v
			continue;
		XMVECTOR disP = XMLoadFloat3(&coin[i].pos) - XMLoadFloat3(&pos);
		disP = XMVector3Length(disP);		//������2��
		float point;
		XMStoreFloat(&point, disP);
		if (point < 1.0f)			//���W������1.0�ȉ��Ȃ�]���ő�l���o��
		{
			value += valueCoe[CLONE_ITEM_COIN];
		}
		else
		{
			point = 1.0f / point;					//����Ă���قǌW�����������Ȃ�悤�ɂ���B
			value += valueCoe[CLONE_ITEM_COIN] * point;	//�]���W���Ƌ����̕]������Z
		}
	}

	for (int i = 0; i < MAX_EXTEN; i++)
	{
		if (valueCoe[CLONE_ITEM_EXTEN] == 0.0f || exten[i].use != TRUE)		//�]���W����0�������̓t�B�[���h�ɖ����Ȃ�v�Z�s�v
			continue;
		XMVECTOR disP = XMLoadFloat3(&exten[i].pos) - XMLoadFloat3(&pos);
		disP = XMVector3Length(disP);			//������2��
		float point;
		XMStoreFloat(&point, disP);
		if (point < 1.0f)			//���W������1.0�ȉ��Ȃ�]���ő�l���o��
		{
			value += valueCoe[CLONE_ITEM_EXTEN];
		}
		else
		{
			point = 1.0f / point;			//����Ă���قǌW�����������Ȃ�悤�ɂ���B
			value += valueCoe[CLONE_ITEM_EXTEN] * point;	//�]���W���Ƌ����̕]������Z
		}
	}

	if (valueCoe[CLONE_ENEMY] == 0.0f)			//�]���W����0�Ȃ�v�Z�s�v�B���̂܂ܕԂ�
		return value;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE)continue;

		XMVECTOR disP = XMLoadFloat3(&enemy[i].pos) - XMLoadFloat3(&pos);
		disP = XMVector3Length(disP);				//������2��
		float point;
		XMStoreFloat(&point, disP);
		if (point < 1.0f)				//���W������1.0�ȉ��Ȃ�]���ő�l���o��
		{
			value += valueCoe[CLONE_ENEMY];
		}
		else
		{
			point = 1.0f / point;						//����Ă���قǌW�����������Ȃ�悤�ɂ���B
			value += valueCoe[CLONE_ENEMY] * point;		//�]���W���Ƌ����̕]������Z
		}
	}
	//���E���̕]���l����Z
	if (CloneVisibility(clone[k].pos, pos, clone[k].rot.y, CLONE_POINT_INTERVAL * CLONE_POINT_INTERVAL) == TRUE)
	{
		value *= EYE_VALUE_TPS;
	}

	return value;
}

//���ڕ]���@�ɂ��]���̏d�݂Â��Ƃ���Ɋ�Â��I�u�W�F�N�g���̕]���W�����Z�o�B
//(���ꂼ��̃I�u�W�F�N�g�ɑ΂���_���̍��v) / (�]�������I�u�W�F�N�g��)
//�ǂ����������i�ɂ���̂������߂邽�ߔz��ԍ��������Ă���
void ValueCloneCoefficient(int k)
{
	CLONE *clone = GetClone();
	float value = 1.0f;			//(���a�W��)
	float coinValue = 0.0f;		//�R�C���ɑ΂��鏉���̏d��
	float extenValue = 0.0f;	//���ԉ����A�C�e���ɑ΂��鏉���̏d��
	float playerValue = 0.0f;	//���БΏۂɑ΂��鏉���̏d��
	float weight[VALUE_VAR] = { 0.0f, 0.0f, 0.0f };		//�d��
	float sumWeight = 0.0f;
	//�󋵕��͂����A���ꂼ��̕]���ɏd�݂�����

	//���i���Ƃɕ]���ɏd�݂Â�
	if (clone[k].character == CLONE_OFFENSIVE)	//�U���I
	{
		weight[CLONE_ENEMY] += 4.0f;
		weight[CLONE_ITEM_COIN] += 1.0f;
	}
	else if (clone[k].character == CLONE_ITEM)	//���ԃA�C�e���̗D��
	{
		weight[CLONE_ITEM_COIN] += 1.0f;
		weight[CLONE_ITEM_EXTEN] += 4.0f;
	}
	else if (clone[k].character == CLONE_ALL_TAKE)		//�X�R�A�A�C�e���̗D��
	{
		weight[CLONE_ITEM_COIN] += 4.0f;
		weight[CLONE_ITEM_EXTEN] += 1.0f;
		weight[CLONE_ENEMY] += 1.0f;
	}


	sumWeight = weight[CLONE_ITEM_COIN] + weight[CLONE_ITEM_EXTEN] + weight[CLONE_ENEMY];
	valueCoe[CLONE_ITEM_COIN] = weight[CLONE_ITEM_COIN] / sumWeight;		//�R�C���ɑ΂���d��
	valueCoe[CLONE_ITEM_EXTEN] = weight[CLONE_ITEM_EXTEN] / sumWeight;		//���ԉ����A�C�e���ɑ΂���d��
	valueCoe[CLONE_ENEMY] = weight[CLONE_ENEMY] / sumWeight;	//�v���C���[�ɑ΂���d��
}

//��p�I�D�ʉӏ�������
//����1:�E�F�C�|�C���g���W�A����2:�G�̍��W�A����3:�z��ԍ�
float CloneTacticalSuperiority(XMFLOAT3 pos1, XMFLOAT3 pos2,int k)
{
	float ans = 1.0f;	//�܂���1.0�ŏ������B�W���Ƃ��Ďg�p���邽�߁A�����l��1.0
	ROCK *rock = GetRock();
	//��Q���ƃE�F�C�|�C���g�̍��W�̊ԂɓG�����遨�n�C�h�|�C���g�����B�]���W������������
	for (int i = 0; i < MAX_ROCK; i++)
	{
		if ((GetCrossPoints(pos1, pos2, rock[i].pos, ROCK_SIZE) == TRUE))
			ans += 0.1f;
	}

	return ans;
}
/////////////////////////////////////////////////////////////////////
//�I�����ꂽ�E�F�C�|�C���g��ɃG�l�~�[������Ȃ�U������悤�ɂ���B����1:�E�F�C�|�C���g���W�A����2:�N���[���̔z��ԍ�
/////////////////////////////////////////////////////////////////////
void CloneAttack(XMFLOAT3 pos, int k)
{
	ENEMY *enemy = GetEnemy();
	CLONE *clone = GetClone();
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE)continue;	//����ł�G�l�~�[�͒T���s�v

		if (GetCrossPoints(pos, clone[k].pos, enemy[i].pos, enemy[i].size + clone[k].size) != TRUE)
			continue;						//�������ĂȂ��Ȃ玟�̃G�l�~�[��
			
		//�����܂ŗ����瓖�����Ă邽�߁A�U���R�}���h���I���ɂ��A�T���I��
		clone[k].attack = TRUE;
		clone[k].attackUse = TRUE;
		return;
	}
}