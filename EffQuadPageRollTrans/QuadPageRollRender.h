#pragma once

#include "../EffPageRoll/PageRollRender.h"

class CQuadPageRollRender : public CPageRollRender
{
public:
	CQuadPageRollRender(void);
	~CQuadPageRollRender(void);

private:	
	virtual bool Draw(PageRollFxParam* pParam, D3DXMATRIX*	matTex);
};
