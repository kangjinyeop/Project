#ifndef ENGINE_TYPEDEF_H__
#define ENGINE_TYPEDEF_H__

namespace Engine
{
// DirectX 11
	// ���� or ��������
	typedef XMVECTORF32			_FXMfloat32;

#if WINDOWS32_RELEASE
	typedef XMVECTOR			_FXMvec4;
	typedef XMVECTOR&			_CXMvec4;
#elif WINDOWS64_RELEASE	
	typedef XMVECTOR&			_FXMvec4;
	typedef XMVECTOR&			_CXMvec4;
#endif

	// Class�� ��� ����
	typedef XMFLOAT2			_D3D11vec2;
	typedef XMFLOAT3			_D3D11vec3;
	typedef XMFLOAT4			_D3D11vec4;

	typedef XMFLOAT4X4			_D3D11matrix;
}

#endif // ENGINE_TYPEDEF_H__
