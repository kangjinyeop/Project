#include "stdafx.h"
#include "Random.h"

USING(Engine)

IMPLEMENT_SINGLETON(CRandom)

CRandom::CRandom()
	: _State(0), _z0(0), _z1(0), _z2(0)
{
	srand((unsigned)time(NULL));

	for (_INT Index = 0; Index < RANDOM_MAX; ++Index)
	{
		_iRandNumber[Index] = rand() << 16 | rand();
	}
}

CRandom::~CRandom()
{
}

_UINT CRandom::GetValue(_UINT _iMaxValue)
{
	return (_UINT)(ExtractionNumber() * _iMaxValue);
}

_UINT CRandom::GetValue(_UINT _iMiniValue, _UINT _iMaxValue)
{
	return _iMiniValue + (_UINT)(ExtractionNumber() * _iMaxValue);
}

_DOUBLE CRandom::GetDobule(_DOUBLE _iMaxValue)
{
	return ExtractionNumber() * _iMaxValue;
}

_DOUBLE CRandom::GetDobule(_DOUBLE _iMiniValue, _DOUBLE _iMaxValue)
{
	return _iMiniValue + (ExtractionNumber() * _iMaxValue);
}

_DOUBLE CRandom::ExtractionNumber()
{
	_z0 = VRm1;
	_z1 = Identity(V0) ^ MAT0POS(8, VM1);
	_z2 = MAT0NEG(-19, VM2) ^ MAT0NEG(-14, VM3);

	newV1 = _z1 ^ _z2;

	newV0 = MAT0NEG(-11, _z0) ^ MAT0NEG(-7, _z1) ^ MAT0NEG(-13, _z2);

	_State = (_State + 31) & 0x0000001fU;

	return ((double)_iRandNumber[_State] * FACT);
}