//=============================================================================
//
// �����蔻�菈�� [collision.cpp]
// Author : �đq�r�N
//
//=============================================================================
#include "main.h"
#include "collision.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************


//=============================================================================
// BB�ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���TRUE
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// �O����Z�b�g���Ă���

	// ���W�����S�_�Ȃ̂Ōv�Z���₷�������ɂ��Ă���
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// �o�E���f�B���O�{�b�N�X(BB)�̏���
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// �����������̏���
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BC�ɂ�铖���蔻�菈��
// �T�C�Y�͔��a
// �߂�l�F�������Ă���TRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// �O����Z�b�g���Ă���

	float len = (r1 + r2) * (r1 + r2);		// ���a��2�悵����
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2�_�Ԃ̋����i2�悵�����j
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// ���a��2�悵������苗�����Z���H
	if (len > lenSq)
	{
		ans = TRUE;	// �������Ă���
	}

	return ans;
}


//=============================================================================
// ����(dot)
//=============================================================================
float dotProduct(XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	float ans = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
#else
	// �_�C���N�g�w�ł�
	XMVECTOR temp = XMVector3Dot(*v1, *v2);
	float ans = 0.0f;
	XMStoreFloat(&ans, temp);
#endif

	return(ans);
}


//=============================================================================
// �O��(cross)
//=============================================================================
void crossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	ret->x = v1->y * v2->z - v1->z * v2->y;
	ret->y = v1->z * v2->x - v1->x * v2->z;
	ret->z = v1->x * v2->y - v1->y * v2->x;
#else
	// �_�C���N�g�w�ł�
	*ret = XMVector3Cross(*v1, *v2);
#endif

}
//2�_�̐����Ɖ~��������Ă��邩�𒲍�(��p�ʒu��͂Ɏg�p)
//pos1:�n�_	pos2:�I�_ pos3:�~�̒��S�_ r:�~�̔��a
BOOL GetCrossPoints(XMFLOAT3 pos1, XMFLOAT3 pos2, XMFLOAT3 pos3, float r)
{
	BOOL ans = FALSE;
	XMVECTOR tempD = XMLoadFloat3(&pos2) - XMLoadFloat3(&pos1);	//�n�_����I�_�Ɍ������x�N�g��
	XMVECTOR tempB = XMLoadFloat3(&pos3) - XMLoadFloat3(&pos2);	//�I�_����~�̒��S�_�Ɍ������x�N�g��
	XMVECTOR tempS = XMLoadFloat3(&pos3) - XMLoadFloat3(&pos1);	//�n�_����~�̒��S�_�Ɍ������x�N�g��
	XMVECTOR nor;
	XMVECTOR norD;

	//�������~�ɂ܂������Ă���P�[�X�Ɍ��肷��
	XMVECTOR pointv1, pointv2;
	float pointf1, pointf2;
	pointv1 = XMVector3LengthSq(tempB);			// 2�_�Ԃ̋����i2�悵�����j
	pointv2 = XMVector3LengthSq(tempS);
	XMStoreFloat(&pointf1, pointv1);
	XMStoreFloat(&pointf2, pointv2);
	float lenR = r * r;
	//2�_�Ԃ̋����Ɣ��a���r�B���a�̕����傫���Ȃ�[�_���~�����ɓ����Ă��邽�߁A���̂܂ܔ���I���B
	if (pointf1 < lenR || pointf2 < lenR)
		return TRUE;

	//�ˉe���������Ɖ~�̒��S�_�̒��������߂�B���ꂪ�����ƒ��S�_�̋����ƂȂ�
	//�����̃x�N�g����P�ʉ�
	norD = XMVector3Normalize(tempD);
	//�P�ʉ������x�N�g���Ǝn�_���璆�S�_�Ɍ������x�N�g���̊O�ς��Z�o�B�����y�����������ƒ��S�_�̍ŒZ�����ƂȂ�
	crossProduct(&nor, &norD, &tempS);
	XMFLOAT3 distProj;
	XMStoreFloat3(&distProj, nor);
	//�����ƒ��S�_�̋������r�B���a���������Ȃ�Γ�����悤�������̂Ŕ���I��
	if (fabsf(distProj.y) >= r)
		return FALSE;

	//�����ē��όv�Z	
	//�~�������̗��[�̊O���ɂ���ꍇ�A����d�����a�����Z���Ă������ƏՓ˂��Ȃ��ꍇ������
	//tempD��tempS�̓��ς����ꂼ�ꋁ�߂Ċ|���Z���A���ꂪ�v���X�Ȃ�ΐ����ƏՓ˂��Ȃ��Ɣ���ł���
	//2�x�N�g�����s�p�Ȃ�{�A�݊p�Ȃ�-�A���p�Ȃ�O
	float	d1, d2;
	d1 = dotProduct(&tempS, &tempD);
	d2 = dotProduct(&tempB, &tempD);
	//���ς̏�Z���ʂ��قȂ�Ȃ�Փ˂���ȊO�Ȃ炵�Ȃ��B����I��
	if (((d1 * d2) < 0.0f))
		return TRUE;
	

	//�S�����ɓ��Ă͂܂�Ȃ��ꍇ�A�������Ă��Ȃ�����ɂ���(���炭��L�̏����őS�Ēe���Ă���Ƃ͎v��)
	return ans;
}

//=============================================================================
// ���C�L���X�g
// p0, p1, p2�@�|���S���̂R���_
// pos0 �n�_
// pos1 �I�_
// hit�@��_�̕ԋp�p
// normal �@���x�N�g���̕ԋp�p
// �������Ă���ꍇ�ATRUE��Ԃ�
//=============================================================================
BOOL RayCast(XMFLOAT3 xp0, XMFLOAT3 xp1, XMFLOAT3 xp2, XMFLOAT3 xpos0, XMFLOAT3 xpos1, XMFLOAT3 *hit, XMFLOAT3 *normal)
{
	XMVECTOR	p0   = XMLoadFloat3(&xp0);
	XMVECTOR	p1   = XMLoadFloat3(&xp1);
	XMVECTOR	p2   = XMLoadFloat3(&xp2);
	XMVECTOR	pos0 = XMLoadFloat3(&xpos0);
	XMVECTOR	pos1 = XMLoadFloat3(&xpos1);

	XMVECTOR	nor;	// �|���S���̖@��
	XMVECTOR	vec1;
	XMVECTOR	vec2;
	float		d1, d2;

	{	// �|���S���̊O�ς��Ƃ��Ė@�������߂�(���̏����͑Ώۂ��Œ蕨�Ȃ�\��Init()�ōs���Ă����Ɨǂ�)
		vec1 = p1 - p0;
		vec2 = p2 - p0;
		crossProduct(&nor, &vec2, &vec1);
		nor = XMVector3Normalize(nor);		// �v�Z���₷���悤�ɖ@�����m�[�}���C�Y���Ă���(���̃x�N�g���̒������P�ɂ��Ă���)
		XMStoreFloat3(normal, nor);			// ���߂��@�������Ă���
	}

	// �|���S�����ʂƐ����̓��ςƂ��ďՓ˂��Ă���\���𒲂ׂ�i�s�p�Ȃ�{�A�݊p�Ȃ�[�A���p�Ȃ�O�j
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	{	// ���߂��|���S���̖@���ƂQ�̃x�N�g���i�����̗��[�ƃ|���S����̔C�ӂ̓_�j�̓��ςƂ��ďՓ˂��Ă���\���𒲂ׂ�
		d1 = dotProduct(&vec1, &nor);
		d2 = dotProduct(&vec2, &nor);
		if (((d1 * d2) > 0.0f) || (d1 == 0 && d2 == 0))
		{
			// �������Ă���\���͖������߂͂���
			return(FALSE);
		}
	}


	{	// �|���S���Ɛ����̌�_�����߂�
		d1 = (float)fabs(d1);	// ��Βl�����߂Ă���
		d2 = (float)fabs(d2);	// ��Βl�����߂Ă���
		float a = d1 / (d1 + d2);							// ������

		XMVECTOR	vec3 = (1 - a) * vec1 + a * vec2;		// p0�����_�ւ̃x�N�g��
		XMVECTOR	p3 = p0 + vec3;							// ��_
		XMStoreFloat3(hit, p3);								// ���߂���_�����Ă���

		{	// ���߂���_���|���S���̒��ɂ��邩���ׂ�

			// �|���S���̊e�ӂ̃x�N�g��
			XMVECTOR	v1 = p1 - p0;
			XMVECTOR	v2 = p2 - p1;
			XMVECTOR	v3 = p0 - p2;

			// �e���_�ƌ�_�Ƃ̃x�N�g��
			XMVECTOR	v4 = p3 - p1;
			XMVECTOR	v5 = p3 - p2;
			XMVECTOR	v6 = p3 - p0;

			// �O�ςŊe�ӂ̖@�������߂āA�|���S���̖@���Ƃ̓��ς��Ƃ��ĕ������`�F�b�N����
			XMVECTOR	n1, n2, n3;

			crossProduct(&n1, &v4, &v1);
			if (dotProduct(&n1, &nor) < 0.0f) return(FALSE);	// �������Ă��Ȃ�

			crossProduct(&n2, &v5, &v2);
			if (dotProduct(&n2, &nor) < 0.0f) return(FALSE);	// �������Ă��Ȃ�
			
			crossProduct(&n3, &v6, &v3);
			if (dotProduct(&n3, &nor) < 0.0f) return(FALSE);	// �������Ă��Ȃ�
		}
	}

	return(TRUE);	// �������Ă���I(hit�ɂ͓������Ă����_�������Ă���Bnormal�ɂ͖@���������Ă���)
}



