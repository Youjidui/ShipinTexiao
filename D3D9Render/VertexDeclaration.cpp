#include "StdAfx.h"
#include ".\VertexDeclaration.h"
#include "BaseTexture.h"
#include <assert.h>
#include "../Utility/SafeDelete.h"

CVertexDeclaration::CVertexDeclaration()
{
}

CVertexDeclaration::~CVertexDeclaration()
{	
	Destroy();
}
HRESULT CVertexDeclaration::Create ( LPDIRECT3DDEVICE9 pDevice, CONST D3DVERTEXELEMENT9* pVertexElements, LPCTSTR pszName)
{

	m_pDevice		  = pDevice;

	m_strResID          = pszName;

	HRESULT hr = S_OK;
	LPDIRECT3DVERTEXDECLARATION9 pDecl = NULL;
	if( SUCCEEDED(hr =  m_pDevice->CreateVertexDeclaration( pVertexElements, &pDecl ) ) )
	{
		m_pVertexDecl = pDecl;
	}

	return hr;
}

LPDIRECT3DVERTEXDECLARATION9 CVertexDeclaration::GetVertexDeclarationPtr()
{
	return m_pVertexDecl;
}
HRESULT CVertexDeclaration::Destroy()
{
	SAFE_RELEASE(m_pVertexDecl);
	return S_OK;
}
