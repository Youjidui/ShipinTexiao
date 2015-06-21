#pragma once


class  CVertexDeclaration
{
public:
	CVertexDeclaration();
	virtual ~CVertexDeclaration();
	HRESULT Create ( LPDIRECT3DDEVICE9 pDevice, 
		CONST D3DVERTEXELEMENT9* pVertexElements,
		LPCTSTR pszName);
	LPDIRECT3DVERTEXDECLARATION9 GetVertexDeclarationPtr();
protected:
	HRESULT Destroy();

private:
	LPDIRECT3DDEVICE9 m_pDevice;
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDecl;
	std::wstring m_strResID;
};
