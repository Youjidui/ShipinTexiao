#include "StdAfx.h"
#include "ShaderResources.h"
#include "../Utility/PathSettings.h"
#include "../Logger/Logging.h"
#include "../Logger/LogDxError.h"
#include <memory>

//#include <DxErr.h>
//#pragma comment(lib, "DxErr")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")

//#include <map>
#include "../zip_utils_src/unzip.h"

class CShaderResources
{
private:
	HZIP m_hzip;
	//typedef std::map<CString, ZIPENTRY> ZipInfoMap_t;
	//ZipInfoMap_t m_zipmap;

public:
	CShaderResources(void);
	~CShaderResources(void);

	LPVOID GetShaderBuffer(LPCTSTR lpszShaderName, int& nBufferSize);
};

CShaderResources::CShaderResources(void)
{
	TCHAR szExeFilePath[MAX_PATH];
	PathSettings::BuildResourcePath(szExeFilePath, sizeof(szExeFilePath), _T("shaders.o"));
	ASSERT(PathFileExists(szExeFilePath));

	HZIP hz = OpenZip(szExeFilePath, 0);
	if (hz) 
	{
		m_hzip = hz;
		//ZIPENTRY ze;
		//GetZipItem(hz,-1,&ze);
		//int numitems=ze.index;
		//for (int zi=0; zi<numitems; zi++)
		//{
		//	GetZipItem(hz,zi,&ze);
		//	//UnzipItem(hz,zi,ze.name);
		//	m_zipmap[ze.name] = ze;
		//}
	}
	else
	{
		CHECK_AND_LOG_WINDOWS_API_ERROR();
	} 
}

CShaderResources::~CShaderResources(void)
{
	if(m_hzip)
		CloseZip(m_hzip);
}

LPVOID CShaderResources::GetShaderBuffer( LPCTSTR lpszShaderName, int& nBufferSize )
{
	LPVOID pRet = NULL;
	//ZipInfoMap_t::iterator i = m_zipmap.find(lpszShaderName);
	//if(m_zipmap.end() != i)
	//{
	//	const ZIPENTRY& entry = i->second;
	//	UnzipItem(m_hzip, entry.index, entry.name);
	//}
	if(m_hzip)
	{
		int index = -1;
		ZIPENTRY ze;
		FindZipItem(m_hzip, lpszShaderName, true, &index, &ze);
		if(0 <= index)
		{
			ASSERT(ze.unc_size > 0);
			char *ibuf = new char[ze.unc_size];
			if(ZR_OK == UnzipItem(m_hzip, index, ibuf, ze.unc_size))
			{
				pRet = ibuf;
				nBufferSize = ze.unc_size;
			}
			else
				delete[] ibuf;
		}
	}
	return pRet;
}

LPVOID GetShaderBufferAndSize( LPCTSTR lpszShaderName, int& nBufferSize )
{
	std::auto_ptr<CShaderResources> s_ShaderResources;
	if(NULL == s_ShaderResources.get())
	{
		s_ShaderResources.reset(new CShaderResources);
	}
	nBufferSize = 0;
	if(s_ShaderResources.get())
	{
		return s_ShaderResources->GetShaderBuffer(lpszShaderName, nBufferSize);
	}
	return NULL;
}
