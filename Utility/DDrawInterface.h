#pragma once

#include <ddraw.h>

class DDrawInterface
{
public:
	DDrawInterface(void);
	~DDrawInterface(void);

	LPDIRECTDRAW7 GetDDraw() {return m_pddraw7;}

private:
	HMODULE				g_hDDrawLib;
	LPDIRECTDRAW7         m_pddraw7;
	void  _util_initialize_ddraw7();
	void _util_finalize_ddraw7();
};
