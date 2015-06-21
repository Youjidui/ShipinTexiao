#pragma once

#include <map>
#include <string>

template<class T>
class TResourceCollection
{
public:

	TResourceCollection(void)	{	}

	~TResourceCollection(void)	{DeleteAll();	}

	T* Create(LPDIRECT3DDEVICE9 pDevice, LPCTSTR pszShaderFileName);

	bool Delete(LPCTSTR pszShaderFileName);
	void DeleteAll();

private:
	typedef std::map<std::wstring, T*> ResourceMap;
	ResourceMap m_items;
};


template<class T>
T* TResourceCollection<T>::Create( LPDIRECT3DDEVICE9 pDevice, LPCTSTR pszFilename )
{
	T* pShader = NULL;
	ResourceMap::iterator i = m_items.find(pszFilename);
	if(m_items.end() != i)
	{
		pShader = i->second;
	}
	else
	{
		pShader = new T;
		pShader->Create(pDevice, pszFilename);
		std::pair<ResourceMap::iterator, bool> ret = m_items.insert(std::make_pair(pszFilename, pShader));
		assert(ret.second);
	}
	return pShader;
}

template<class T>
bool TResourceCollection<T>::Delete( LPCTSTR pszShaderFileName )
{
	bool bFound = false;
	ResourceMap::iterator i = m_items.find(pszShaderFileName);
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
