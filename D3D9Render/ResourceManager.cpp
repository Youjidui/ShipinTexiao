#include "StdAfx.h"
#include "ResourceManager.h"

CResourceManager::CResourceManager(void)
{
}

CResourceManager::~CResourceManager(void)
{
}

CBaseFx* CResourceManager::CreateEffect( LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszEffectName )
{
	return m_FxRes.Create(lpDevice, lpszEffectName);
}

CVertexShader* CResourceManager::CreateVertexShader( LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName )
{
	return m_VSRes.Create(lpDevice, lpszShaderName);
}

CPixelShader* CResourceManager::CreatePixelShader( LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName )
{
	return m_PSRes.Create(lpDevice, lpszShaderName);
}
