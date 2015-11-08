#include "StdAfx.h"
#include "ChromaKeyRender.h"

ChromaKeyRender::ChromaKeyRender(void)
: m_PS_CRK_RGB32(NULL)
{
}

ChromaKeyRender::~ChromaKeyRender(void)
{
}

bool ChromaKeyRender::Init( CRenderEngine* pEngine )
{
	//CSonyMaskRender::Init(pEngine);

	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();
	m_PS_CRK_RGB32 = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_CRK_RGB32.psh"));
	ASSERT(m_PS_CRK_RGB32);
	return true;
}
