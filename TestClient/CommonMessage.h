#pragma once

#define UM_SET_IMAGE (WM_USER + 600)
//����ͼ���ͼ���ļ�
//WPARAM:UINT		= ͼ���ţ���0��ʼ
//LParam:LPCTSTR	= ͼ���ļ���������·����
 
#define UM_SET_TARGET_BUFFER_SIZE (WM_USER + 601)
//����Ŀ��Buffer�Ĵ�С
//WPARAM:UINT		= Width
//LPARAM:UINT		= Height

#define UM_SELECT_EFFECT (WM_USER + 602)
//ĳ����Ƶ��Ч��ѡ��
//WPARAM: LPCTSTR = effect Name
//LPARAM: FxParamBase*  = effect parameter

#define UM_EFFECT_PARAMETER_CHANGE (WM_USER + 603)
//ĳ����Ч�Ĳ��������ı�
//WPARAM: LPCTSTR = effect Name
//LPARAM: FxParamBase* = ��Ч�����ṹ����ָ��
