#pragma once

#define UM_SET_IMAGE (WM_USER + 600)
//设置图层的图像文件
//WPARAM:UINT		= 图层编号，从0开始
//LParam:LPCTSTR	= 图像文件名（包含路径）
 
#define UM_SET_TARGET_BUFFER_SIZE (WM_USER + 601)
//设置目标Buffer的大小
//WPARAM:UINT		= Width
//LPARAM:UINT		= Height

#define UM_SELECT_EFFECT (WM_USER + 602)
//某个视频特效被选择
//WPARAM: LPCTSTR = effect Name
//LPARAM: FxParamBase*  = effect parameter

#define UM_EFFECT_PARAMETER_CHANGE (WM_USER + 603)
//某个特效的参数发生改变
//WPARAM: LPCTSTR = effect Name
//LPARAM: FxParamBase* = 特效参数结构变量指针
