#include "stdafx.h"
#include "Component.h"

USING(Engine)

CComponent::CComponent(void)
{
}

CComponent::~CComponent(void)
{
}

HRESULT CComponent::Initialize(void)
{
	return S_OK;
}

void CComponent::Update(const double & TimePerSec)
{
}

void CComponent::D3D9Render(LPDIRECT3DDEVICE9 Device)
{
}

void CComponent::D3D11Render(LPDIRECT3DDEVICE9 Device)
{
}