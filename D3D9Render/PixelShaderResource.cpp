#include "StdAfx.h"
#include "PixelShaderResource.h"
/*
CPixelShaderResource::CPixelShaderResource(void)
{
}

CPixelShaderResource::~CPixelShaderResource(void)
{
}

CPixelShader* CPixelShaderResource::CreateShader( LPDIRECT3DDEVICE9* pDevice, LPCTSTR pszFilename )
{
	CPixelShader* pShader = NULL;
	ShaderMap::iterator i = m_shaders.find(pszFilename);
	if(m_shaders.end() != i)
	{
		pShader = i->second;
	}
	else
	{
		pShader = new CPixelShader;
		pShader->Create(pShader);
		std::pair<ShaderMap::iterator, bool> ret = m_shaders.insert(std::make_pair(pszFilename, pShader));
		assert(ret.second);
	}
	return pShader;
}

bool CPixelShaderResource::DeleteShader( LPCTSTR pszShaderFileName )
{
	bool bFound = false;
	ShaderMap::iterator i = m_shaders.find(pszShaderFileName);
	if(m_shaders.end() != i)
	{
		CPixelShader* pShader = i->second;
		delete pShader;
		m_shaders.erase(i);
		bFound = true;
	}
	return bFound;
}

void CPixelShaderResource::DeleteAll()
{
	for(ShaderMap::iterator i = m_shaders.begin(); i != m_shaders.end(); ++i)
	{
		CPixelShader* pShader = i->second;
		delete pShader;
	}
	m_shaders.clear();
}
*/