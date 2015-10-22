#pragma once

#include <d3dx9.h>
#include "BaseMesh.h"
#include "ResourceCollection.h"

class CMeshResource : public TResourceCollection<CBaseMesh>
{
	typedef TResourceCollection<CBaseMesh> _baseclass;
public:
	struct CreateByMeshFunction
	{
		LPDIRECT3DDEVICE9 m_pDevice;
		LPD3DXMESH m_pMesh;
		LPCTSTR m_pszResFileName;

		CreateByMeshFunction(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPCTSTR pszResFileName)
			: m_pDevice(pDevice), m_pMesh(pMesh), m_pszResFileName(pszResFileName)
		{
		}

		HRESULT operator()(CBaseMesh* pMesh, LPCTSTR pszResFileName)
		{
			return pMesh->Create(m_pDevice, m_pMesh, pszResFileName);
		}
	};

	CBaseMesh* Create(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPCTSTR pszResFileName)
	{
		CreateByMeshFunction creator(pDevice, pMesh, pszResFileName);
		return _baseclass::Create(creator, pszResFileName);
	}

	struct CreateByVertexBufferFunction
	{
		LPDIRECT3DDEVICE9 m_pDevice;
		LPDIRECT3DVERTEXBUFFER9* m_ppVB2;
		LPDIRECT3DINDEXBUFFER9  m_pIB;
		LPDIRECT3DVERTEXDECLARATION9 m_pDcl;
		LPCTSTR                m_pszResID;

		CreateByVertexBufferFunction(LPDIRECT3DDEVICE9 pDevice,
			LPDIRECT3DVERTEXBUFFER9* ppVB,
			LPDIRECT3DINDEXBUFFER9  pIB,
			LPDIRECT3DVERTEXDECLARATION9 pDcl,
			LPCTSTR                pszResID)
			: m_pDevice(pDevice), m_ppVB2(ppVB), m_pIB(pIB), m_pDcl(pDcl), m_pszResID(pszResID)
		{
		}

		HRESULT operator()(CBaseMesh* pMesh, LPCTSTR pszResFileName)
		{
			return pMesh->Create(m_pDevice, m_ppVB2, m_pIB, m_pDcl, pszResFileName);
		}
	};

	CBaseMesh* Create(LPDIRECT3DDEVICE9 pDevice,
		LPDIRECT3DVERTEXBUFFER9* ppVB,
		LPDIRECT3DINDEXBUFFER9  pIB,
		LPDIRECT3DVERTEXDECLARATION9 pDcl,
		LPCTSTR                pszResID)
	{
		CreateByVertexBufferFunction creator(pDevice, ppVB, pIB, pDcl, pszResID);
		return _baseclass::Create(creator, pszResID);
	}
};

