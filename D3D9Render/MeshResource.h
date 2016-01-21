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
		LPDIRECT3DVERTEXDECLARATION9 m_pVertexDecl;
		LPCTSTR m_pszResFileName;

		CreateByMeshFunction(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPCTSTR pszResFileName)
			: m_pDevice(pDevice), m_pMesh(pMesh), m_pVertexDecl(NULL), m_pszResFileName(pszResFileName)
		{
		}

		CreateByMeshFunction(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPDIRECT3DVERTEXDECLARATION9 pVertexDecl, LPCTSTR pszResFileName)
			: m_pDevice(pDevice), m_pMesh(pMesh), m_pVertexDecl(pVertexDecl), m_pszResFileName(pszResFileName)
		{
		}

		HRESULT operator()(CBaseMesh* pMesh, LPCTSTR pszResFileName)
		{
			return pMesh->Create(m_pDevice, m_pMesh, m_pVertexDecl, pszResFileName);
		}
	};

	//create function
	CBaseMesh* Create(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPCTSTR pszResFileName)
	{
		CreateByMeshFunction creator(pDevice, pMesh, pszResFileName);
		return _baseclass::Create(creator, pszResFileName);
	}

	//create function
	CBaseMesh* Create(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPDIRECT3DVERTEXDECLARATION9 pVertexDecl, LPCTSTR pszResFileName)
	{
		CreateByMeshFunction creator(pDevice, pMesh, pVertexDecl, pszResFileName);
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

	//create function
	CBaseMesh* Create(LPDIRECT3DDEVICE9 pDevice,
		LPDIRECT3DVERTEXBUFFER9* ppVB,
		LPDIRECT3DINDEXBUFFER9  pIB,
		LPDIRECT3DVERTEXDECLARATION9 pDcl,
		LPCTSTR                pszResID)
	{
		CreateByVertexBufferFunction creator(pDevice, ppVB, pIB, pDcl, pszResID);
		return _baseclass::Create(creator, pszResID);
	}

	struct CreateByVertexMemoryFunction
	{
		LPDIRECT3DDEVICE9 m_pDevice;
		void*               m_pVertexInfo;
		UINT                m_uVBufferSize;
		UINT                m_uVertexStride;
		D3DPRIMITIVETYPE    m_ePrimitiveType;
		UINT                m_uPrimitiveCount;
		DWORD               m_dwFVF;
		const D3DVERTEXELEMENT9*  m_pVertexElement;
		const unsigned short**    m_ppIndexInfo;
		UINT                m_uIBufferSize;
		UINT                m_uIBNum;
		LPCTSTR             m_pszResID;

		CreateByVertexMemoryFunction(LPDIRECT3DDEVICE9 pDevice,
			void*               pVertexInfo,
			UINT                uVBufferSize,
			UINT                uVertexStride,
			D3DPRIMITIVETYPE    ePrimitiveType,
			UINT                uPrimitiveCount,
			DWORD               dwFVF,
			const D3DVERTEXELEMENT9*  pVertexElement,
			const unsigned short**     ppIndexInfo,
			UINT                 uIBufferSize,
			UINT                 uIBNum,
			LPCTSTR                pszResID)
			: m_pDevice(pDevice), m_pVertexInfo(pVertexInfo), m_uVBufferSize(uVBufferSize), m_uVertexStride(uVertexStride)
			, m_ePrimitiveType(ePrimitiveType), m_uPrimitiveCount(uPrimitiveCount), m_dwFVF(dwFVF), m_pVertexElement(pVertexElement)
			, m_ppIndexInfo(ppIndexInfo), m_uIBufferSize(uIBufferSize), m_uIBNum(uIBNum), m_pszResID(pszResID)
		{
		}

		HRESULT operator()(CBaseMesh* pMesh, LPCTSTR pszResFileName)
		{
			return pMesh->Create(m_pDevice, m_pVertexInfo, m_uVBufferSize, m_uVertexStride
				, m_ePrimitiveType, m_uPrimitiveCount, m_dwFVF, m_pVertexElement
				, m_ppIndexInfo, m_uIBufferSize, m_uIBNum, pszResFileName);
		}
	};

	//create function
	CBaseMesh* Create(LPDIRECT3DDEVICE9 pDevice,
		void*               pVertexInfo,
		UINT                uVBufferSize,
		UINT                uVertexStride,
		D3DPRIMITIVETYPE    ePrimitiveType,
		UINT                uPrimitiveCount,
		DWORD               dwFVF,
		const D3DVERTEXELEMENT9*  pVertexElement,
		const unsigned short**     ppIndexInfo,
		UINT                 uIBufferSize,
		UINT                 uIBNum,
		LPCTSTR          pszResID)
	{
		CreateByVertexMemoryFunction creator(pDevice, pVertexInfo, uVBufferSize, uVertexStride
			, ePrimitiveType, uPrimitiveCount, dwFVF, pVertexElement
			, ppIndexInfo, uIBufferSize, uIBNum, pszResID);
		return _baseclass::Create(creator, pszResID);
	}


	struct CreateByIndexedVertexMemoryFunction
	{
		LPDIRECT3DDEVICE9	m_pDevice;
		void*				m_pVertexInfo_indexed;
		UINT                m_uVBufferSize_indexed;
		UINT                m_uVertexStride_indexed;
		void*				m_pVertexInfo_instance;
		UINT				m_uVBufferSize_instance;
		UINT				m_uVertexStride_instance;
		D3DPRIMITIVETYPE    m_ePrimitiveType;
		UINT                m_uPrimitiveCount;
		DWORD               m_dwFVF;
		const D3DVERTEXELEMENT9*  m_pVertexElement;
		const unsigned short**    m_ppIndexInfo;
		UINT                m_uIBufferSize;
		UINT                m_uIBNum;
		LPCTSTR             m_pszResID;

		CreateByIndexedVertexMemoryFunction(LPDIRECT3DDEVICE9 pDevice,
			void*               pVertexInfo_indexed,
			UINT                uVBufferSize_indexed,
			UINT                uVertexStride_indexed,
			void*				 pVertexInfo_instance,
			UINT				 uVBufferSize_instance,
			UINT				 uVertexStride_instance,
			D3DPRIMITIVETYPE    ePrimitiveType,
			UINT                uPrimitiveCount,
			DWORD               dwFVF,
			const D3DVERTEXELEMENT9*  pVertexElement,
			const unsigned short**     ppIndexInfo,
			UINT                 uIBufferSize,
			UINT                 uIBNum,
			LPCTSTR                pszResID)
			: m_pDevice(pDevice)
			, m_pVertexInfo_indexed(pVertexInfo_indexed), m_uVBufferSize_indexed(uVBufferSize_indexed), m_uVertexStride_indexed(uVertexStride_indexed)
			, m_pVertexInfo_instance(pVertexInfo_instance), m_uVBufferSize_instance(uVBufferSize_instance), m_uVertexStride_instance(uVertexStride_instance)
			, m_ePrimitiveType(ePrimitiveType), m_uPrimitiveCount(uPrimitiveCount), m_dwFVF(dwFVF), m_pVertexElement(pVertexElement)
			, m_ppIndexInfo(ppIndexInfo), m_uIBufferSize(uIBufferSize), m_uIBNum(uIBNum), m_pszResID(pszResID)
		{
		}

		HRESULT operator()(CBaseMesh* pMesh, LPCTSTR pszResFileName)
		{
			return pMesh->Create(m_pDevice
				, m_pVertexInfo_indexed, m_uVBufferSize_indexed, m_uVertexStride_indexed
				, m_pVertexInfo_instance, m_uVBufferSize_instance, m_uVertexStride_instance
				, m_ePrimitiveType, m_uPrimitiveCount, m_dwFVF, m_pVertexElement
				, m_ppIndexInfo, m_uIBufferSize, m_uIBNum, pszResFileName);
		}
	};

	//create function
	CBaseMesh* Create(LPDIRECT3DDEVICE9 pDevice,
		void*               pVertexInfo_indexed,
		UINT                uVBufferSize_indexed,
		UINT                uVertexStride_indexed,
		void*				 pVertexInfo_instance,
		UINT				 uVBufferSize_instance,
		UINT				 uVertexStride_instance,
		D3DPRIMITIVETYPE    ePrimitiveType,
		UINT                uPrimitiveCount,
		DWORD               dwFVF,
		const D3DVERTEXELEMENT9*  pVertexElement,
		const unsigned short**    ppIndexInfo,
		UINT                uIBufferSize,
		UINT                uIBNum,
		LPCTSTR          pszResID)
	{
		CreateByIndexedVertexMemoryFunction creator(pDevice,
			pVertexInfo_indexed,
			uVBufferSize_indexed,
			uVertexStride_indexed,
			pVertexInfo_instance,
			uVBufferSize_instance,
			uVertexStride_instance,
			ePrimitiveType,
			uPrimitiveCount,
			dwFVF,
			pVertexElement,
			ppIndexInfo,
			uIBufferSize,
			uIBNum,
			pszResID);
		return _baseclass::Create(creator, pszResID);
	}
};

