#pragma once

#include "ResourceCollection.h"
#include "PixelShader.h"

typedef TResourceCollection<CPixelShader> CPixelShaderResource;

/*
class CPixelShaderResource
{
public:
	CPixelShaderResource(void);
	~CPixelShaderResource(void);

	CPixelShader* CreateShader(LPDIRECT3DDEVICE9* pDevice, LPCTSTR pszShaderFileName);

	bool DeleteShader(LPCTSTR pszShaderFileName);
	void DeleteAll();

private:
	typedef std::map<std::wstring, CPixelShader*> ShaderMap;
	ShaderMap m_shaders;
};
*/