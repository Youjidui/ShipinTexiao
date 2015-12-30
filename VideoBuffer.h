#pragma once

#include <d3d9types.h>

enum RenderEngineType
{
	RET_D3D9 = 0
};


struct VideoBufferInfo
{
	enum MemType
	{
		SYSTEM_MEM = 0x00,
		VIDEO_MEM   = 0x01,
	};

	enum Usage
	{
		_IN,
		_IN_OUT,
		_OUT,
		_IN_OUT_WITH_MIPMAP
	};

	D3DFORMAT format;				//IN D3DFMT_A8R8G8B8
	MemType eType;		//IN
	Usage eUsage;	//IN
	int nWidth;					//IN
	int nHeight;				//IN
	int nAllocWidth;			//OUT
	int nAllocHeight;			//OUT
};

class CVideoBuffer;
class CVideoBufferManager;


//�ؼ�������ÿ���ؼ����Լ��Ĳ���
struct FxParamBase
{
	unsigned int cbSize;
	char FxType[8];		//int64,����'ShapWipe',ע���ǵ�����
	char FxSubType[8];	//int64,such as 'left'
};

//�����ؼ�
class ITransEffect
{
public:
	 virtual int Render(CVideoBuffer* pDst, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, FxParamBase* pParam) = 0;
};
//�ڲ��ؼ�
class IEffect
{
public:
	virtual int Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, FxParamBase* pParam) = 0;
};

class CRenderEngine;

AFX_EXT_API CVideoBufferManager* CreateVideoBufferManager(CRenderEngine* pEngine);
AFX_EXT_API void ReleaseVideoBufferManager(CVideoBufferManager* p);

AFX_EXT_API CRenderEngine* InitEffectModule(HWND hDeviceWnd, UINT nBackBufferWidth, UINT nBackBufferHeight );
AFX_EXT_API void UninitEffectModule(CRenderEngine* pEngine);
