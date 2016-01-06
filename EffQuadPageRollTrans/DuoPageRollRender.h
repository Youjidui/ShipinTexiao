#pragma once

#include "../EffPageRoll/PageRollRender.h"

class CDuoPageRollRender : public CPageRollRender
{
public:
	CDuoPageRollRender(void);
	~CDuoPageRollRender(void);

private:	
	virtual bool Draw(PageRollFxParam* pParamBase, D3DXMATRIX*	matTex);
};
