#pragma once

#include "VertexShader.h"
#include "ResourceCollection.h"

//typedef TResourceCollection<CVertexShader> CVertexShaderResource;

class CVertexShaderResource : public TResourceCollection<CVertexShader>
{
	typedef TResourceCollection<CVertexShader> _baseclass;
	typedef _baseclass::_element_type* ElementPtrType;
public:
	struct CreateByFileFunction
	{
		LPDIRECT3DDEVICE9 m_pDevice;
		LPCTSTR m_pszResFileName;
		const char** m_ppszMacros;
		int m_nMacroCount;

		CreateByFileFunction(LPDIRECT3DDEVICE9 pDevice, LPCTSTR pszResFileName, const char** ppszMacros, int nMacroCount)
			: m_pDevice(pDevice), m_pszResFileName(pszResFileName), m_ppszMacros(ppszMacros), m_nMacroCount(nMacroCount)
		{
		}

		HRESULT operator()(ElementPtrType pNewObj, LPCTSTR pszResFileName)
		{
			return pNewObj->Create(m_pDevice, pszResFileName, m_ppszMacros, m_nMacroCount);
		}
	};

	ElementPtrType Create(LPDIRECT3DDEVICE9 pDevice, LPCTSTR pszResFileName, const char** ppszMacros, int nMacroCount)
	{
		CreateByFileFunction creator(pDevice, pszResFileName, ppszMacros, nMacroCount);
		return _baseclass::Create(creator, pszResFileName);
	}
};

