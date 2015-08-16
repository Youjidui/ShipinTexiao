#pragma once

#define UM_SET_IMAGE (WM_USER + 600)
//设置图层的图像文件
//WPARAM:UINT		= 图层编号，从0开始
//LParam:LPCTSTR	= 图像文件名（包含路径）
 
#define UM_SET_TARGET_BUFFER_SIZE (WM_USER + 601)
//设置目标Buffer的大小
//WPARAM:UINT		= Width
//LPARAM:UINT		= Height
