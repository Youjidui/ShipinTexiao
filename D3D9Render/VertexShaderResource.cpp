#include "StdAfx.h"
#include "VertexShaderResource.h"
/*
CVertexShaderResource::CVertexShaderResource(void)
{
}

CVertexShaderResource::~CVertexShaderResource(void)
{
	DeleteAll();
}

CVertexShader* CVertexShaderResource::CreateShader( LPDIRECT3DDEVICE9* pDevice, LPCTSTR pszFilename )
{
	CVertexShader* pShader = NULL;
	ShaderMap::iterator i = m_shaders.find(pszFilename);
	if(m_shaders.end() != i)
	{
		pShader = i->second;
	}
	else
	{
		pShader = new CVertexShader;
		pShader->Create(pShader);
		std::pair<ShaderMap::iterator, bool> ret = m_shaders.insert(std::make_pair(pszFilename, pShader));
		assert(ret.second);
	}
	return pShader;
}

bool CVertexShaderResource::DeleteShader( LPCTSTR pszShaderFileName )
{
	bool bFound = false;
	ShaderMap::iterator i = m_shaders.find(pszShaderFileName);
	if(m_shaders.end() != i)
	{
		CVertexShader* pShader = i->second;
		delete pShader;
		m_shaders.erase(i);
		bFound = true;
	}
	return bFound;
}

void CVertexShaderResource::DeleteAll()
{
	for(ShaderMap::iterator i = m_shaders.begin(); i != m_shaders.end(); ++i)
	{
		CVertexShader* pShader = i->second;
		delete pShader;
	}
	m_shaders.clear();
}
*/