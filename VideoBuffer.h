#pragma once

enum RenderEngineType
{
	RET_D3D9 = 0
};

enum VideoBufferType
{
	VBT_SYSTEM_MEM = 0x00,
	VBT_D3D9_MEM   = 0x01,
	VBT_D3D11_MEM   = 0x02,
};

struct VideoBufferInfo
{
	VideoBufferType eType;
	int nWidth;
	int nHeight;
	DWORD format;
};

class IVideoBuffer
{
public:
	virtual const VideoBufferInfo& GetVideoBufferInfo() const = 0;

	virtual void* LockBuffer(int &Pitch) = 0;
	virtual BOOL UnLockBuffer() = 0;
};

class IVideoBufferManager
{
public:
	virtual IVideoBuffer* CreateVideoBuffer(const VideoBufferInfo &info) = 0;
	virtual BOOL ReleaseVideoBuffer(IVideoBuffer*& pBuffer) = 0;
};


//特技参数，每个特技有自己的参数
struct tagFxParamBase
{
	unsigned int cbSize;
	char FxType[8];		//int64,形如'ShapWipe',注意是单引号
	char FxSubType[8];	//int64,such as 'left'
};

//过渡特技
class ITransEffect
{
public:
	 virtual int Render(IVideoBuffer* pSrcA, IVideoBuffer* pSrcB, IVideoBuffer* pDst, tagFxParamBase* pParam) = 0;
};
//内部特技
class IEffect
{
public:
	virtual int Render(IVideoBuffer* pSrc,IVideoBuffer* pDst,tagFxParamBase* pParam) = 0;
};

AFX_EXT_API IVideoBufferManager* CreateVideoBufferManager();
AFX_EXT_API void ReleaseVideoBufferManager(IVideoBufferManager* p);

AFX_EXT_API bool InitEffectModule(HWND hDeviceWnd, UINT nBackBufferWidth, UINT nBackBufferHeight );
AFX_EXT_API void UninitEffectModule();
