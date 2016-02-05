#pragma once

#include "ResourceCollection.h"
#include "BaseTexture.h"

class CTextureResource : public TResourceCollection<CBaseTexture>
{
	typedef TResourceCollection<CBaseTexture> _baseclass;
	typedef _baseclass::_element_type* ElementPtrType;
public:
	struct CreateByFileFunction
	{
		LPDIRECT3DDEVICE9 m_pDevice;
		LPCTSTR m_pszResFileName;
		UINT         m_nWidth;
		UINT         m_nHeight;
		DWORD        m_dwUsage;
		D3DFORMAT    m_Format;
		D3DPOOL      m_Pool;
		//int         m_nTexType;	// TPBaseTexType_Tex=0, TPBaseTexType_Volume=1, TPBaseTexType_Cube=2
		LPCTSTR     m_pszResName;

		CreateByFileFunction(LPDIRECT3DDEVICE9 pDevice,
			LPCTSTR		pszResFileName,
			UINT        nWidth,
			UINT        nHeight,
			DWORD       dwUsage,
			D3DFORMAT   Format,
			D3DPOOL     Pool,
			//int         nTexType,	// TPBaseTexType_Tex=0, TPBaseTexType_Volume=1, TPBaseTexType_Cube=2
			LPCTSTR     pszResName
			)
			: m_pDevice(pDevice), m_pszResFileName(pszResFileName), m_nWidth(nWidth), m_nHeight(nHeight)
			, m_dwUsage(dwUsage), m_Format(Format), m_Pool(Pool), m_pszResName(pszResName)
		{
		}

		HRESULT operator()(ElementPtrType pNewObj, LPCTSTR pszResFileName)
		{
			return pNewObj->Create(m_pDevice, m_pszResFileName, m_nWidth, m_nHeight
				, m_dwUsage, m_Format, m_Pool, m_pszResName);
		}
	};

	ElementPtrType Create(LPDIRECT3DDEVICE9 pDevice,
		LPCTSTR		 pszResFileName,
		UINT         nWidth,
		UINT         nHeight,
		DWORD        dwUsage,
		D3DFORMAT    Format,
		D3DPOOL      Pool,
		//int         nTexType,	// TPBaseTexType_Tex=0, TPBaseTexType_Volume=1, TPBaseTexType_Cube=2
		LPCTSTR     pszResName
		)
	{
		CreateByFileFunction creator(pDevice, pszResFileName, nWidth, nHeight
			, dwUsage, Format, Pool, pszResName);
		return _baseclass::Create(creator, pszResFileName);
	}
};

