#pragma once

#include <map>
#include <string>
#include <assert.h>

template<class T>
class TResourceCollection
{
public:
	typedef T _element_type;

	TResourceCollection(void)	{	}

	~TResourceCollection(void)	{DeleteAll();	}

	template<class F>
	T* Create(F CreateFunction, LPCTSTR pszResFileName);
	//T* Create(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPCTSTR pszResFileName);	//for Mesh resources
	//T* Create(LPDIRECT3DDEVICE9 pDevice, LPCTSTR pszResFileName, const char** ppszMacros, int nMacroCount);	//for shaders

	T* Find( LPCTSTR pszFileName );
	bool Delete(LPCTSTR pszResFileName);
	void DeleteAll();

private:
#ifdef _UNICODE
	typedef std::map<std::wstring, T*> ResourceMap;
#else
	typedef std::map<std::string, T*> ResourceMap;
#endif
	ResourceMap m_items;
	//CCriticalSection m_cs;
};


//template<class T>
//T* TResourceCollection<T>::Create( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pszFilename )
//{
//	T* pRes = NULL;
//	ResourceMap::iterator i = m_items.find(pszFilename);
//	if(m_items.end() != i)
//	{
//		pRes = i->second;
//	}
//	else
//	{
//		T* pNew = new T;
//		HRESULT hr = pNew->Create(pDevice, pszFilename);
//		if(SUCCEEDED(hr))
//		{
//			std::pair<ResourceMap::iterator, bool> ret = m_items.insert(std::make_pair(pszFilename, pNew));
//			assert(ret.second);
//			pRes = pNew;
//		}
//		else
//		{
//			delete pNew;
//		}
//	}
//	return pRes;
//}

template<class T>
template<class F>
T* TResourceCollection<T>::Create( F CreateFunction, LPCTSTR pszFilename )
{
	T* pRes = NULL;
	ResourceMap::iterator i = m_items.find(pszFilename);
	if(m_items.end() != i)
	{
		pRes = i->second;
	}
	else
	{
		T* pNew = new T;
		HRESULT hr = CreateFunction(pNew, pszFilename);
		if(SUCCEEDED(hr))
		{
			std::pair<ResourceMap::iterator, bool> ret = m_items.insert(std::make_pair(pszFilename, pNew));
			assert(ret.second);
			pRes = pNew;
		}
		else
		{
			delete pNew;
		}
	}
	return pRes;
}

//template<class T>
//T* TResourceCollection<T>::Create( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pszFilename, const char** ppszMacros, int nMacroCount )
//{
//	T* pShader = NULL;
//	ResourceMap::iterator i = m_items.find(pszFilename);
//	if(m_items.end() != i)
//	{
//		pShader = i->second;
//	}
//	else
//	{
//		T* pNewShader = new T;
//		HRESULT hr = pNewShader->Create(pDevice, pszFilename, ppszMacros, nMacroCount);
//		if(SUCCEEDED(hr))
//		{
//			std::pair<ResourceMap::iterator, bool> ret = m_items.insert(std::make_pair(pszFilename, pNewShader));
//			assert(ret.second);
//			pShader = pNewShader;
//		}
//		else
//		{
//			delete pNewShader;
//		}
//	}
//	return pShader;
//}

template<class T>
T* TResourceCollection<T>::Find( LPCTSTR pszFileName )
{
	T* pShader = NULL;
	ResourceMap::iterator i = m_items.find(pszFileName);
	if(m_items.end() != i)
	{
		pShader = i->second;
	}
	return pShader;
}

template<class T>
bool TResourceCollection<T>::Delete( LPCTSTR pszFileName )
{
	bool bFound = false;
	ResourceMap::iterator i = m_items.find(pszFileName);
	if(m_items.end() != i)
	{
		T* pShader = i->second;
		delete pShader;
		m_items.erase(i);
		bFound = true;
	}
	return bFound;
}

template<class T>
void TResourceCollection<T>::DeleteAll()
{
	for(ResourceMap::iterator i = m_items.begin(); i != m_items.end(); ++i)
	{
		T* pShader = i->second;
		delete pShader;
	}
	m_items.clear();
}
