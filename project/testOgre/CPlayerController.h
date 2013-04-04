#pragma once
#include "stdafx.h"

class CPlayerController :
	public CForceController
{
public:
	CPlayerController(char RaceType);
	virtual ~CPlayerController(void);
	void OnMousePressed(CIsland *);
    void OnMouseReleased(CIsland *);
	void OnMouseMoved(bool LButtonDown,CIsland*);
	std::set<CGas*>& GetSelectedGas(void);
private:
    std::set<CGas*> m_SelectedGas;
	void SetTargetOfSelectedGas(CIsland *);
    bool m_bMovedAfterCorresponedingMousePressEvent;
};
