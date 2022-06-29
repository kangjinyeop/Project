#ifndef RANDOM_H__
#define RANDOM_H__

BEGIN(Engine)

class ENGINE_DLL CRandom
{
	DECLARE_SINGLETON(CRandom)
private:
#define RANDOM_MAX		32
#define W				32
#define M1				3
#define M2				24
#define M3				10
#define MAT0POS(t,v)	(v^(v>>t))
#define MAT0NEG(t,v)	(v^(v<<(-(t))))
#define Identity(v)		(v)
#define V0				_iRandNumber[_State]
#define VM1				_iRandNumber[(_State + M1) & 0x0000001fU]
#define VM2				_iRandNumber[(_State + M2) & 0x0000001fU]
#define VM3				_iRandNumber[(_State + M3) & 0x0000001fU]
#define VRm1			_iRandNumber[(_State + 31) & 0x0000001fU]
#define newV0			_iRandNumber[(_State + 31) & 0x0000001fU]
#define newV1			_iRandNumber[_State]
#define FACT			2.32830643653869628906e-10
private:
	explicit CRandom();
public:
	~CRandom();
public:
	_UINT		GetValue(_UINT _iMaxValue = 1);
	_UINT		GetValue(_UINT _iMiniValue, _UINT _iMaxValue);
	_DOUBLE		GetDobule(_DOUBLE _iMaxValue = 1);
	_DOUBLE		GetDobule(_DOUBLE _iMiniValue, _DOUBLE _iMaxValue);
private:	
	_DOUBLE		ExtractionNumber();
private:
	_UINT		_iRandNumber[RANDOM_MAX];	
	_UINT		_State;
	_UINT		_z0;
	_UINT		_z1;
	_UINT		_z2;
};

END

#endif // !RANDOM_H__