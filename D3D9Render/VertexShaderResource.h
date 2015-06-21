#pragma once

#include "VertexShader.h"
#include "ResourceCollection.h"

typedef TResourceCollection<CVertexShader> CVertexShaderResource;

/*
#include <map>
#include <string>

class CVertexShaderResource
{
public:
	CVertexShaderResource(void);
	~CVertexShaderResource(void);

	CVertexShader* CreateShader(LPDIRECT3DDEVICE9* pDevice, LPCTSTR pszShaderFileName);

	bool DeleteShader(LPCTSTR pszShaderFileName);
	void DeleteAll();

private:
	typedef std::map<std::wstring, CVertexShader*> ShaderMap;
	ShaderMap m_shaders;
};
*/
